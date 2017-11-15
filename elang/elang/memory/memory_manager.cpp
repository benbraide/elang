#include "../vm/runtime.h"

#include "memory_manager.h"

void elang::memory::manager::boot(){
	access_protected_ = write_protected_ = range_type{};
	next_address_ = 1u;
	map_.clear();
	available_.clear();
}

void elang::memory::manager::protect_from_access(const range_type &range){
	access_protected_ = range;
	if (next_address_ <= range.to)
		next_address_ = (range.to + 1);
}

bool elang::memory::manager::is_access_protected(const range_type &range) const{
	if (elang::vm::runtime::is_inside_protected_mode())
		return false;//Inside protected mode

	return ((access_protected_.from <= range.from && range.from <= access_protected_.to) ||
		(access_protected_.from <= range.to && range.to <= access_protected_.to));
}

void elang::memory::manager::protect_from_write(const range_type &range){
	write_protected_ = range;
}

bool elang::memory::manager::is_write_protected(const range_type &range) const{
	return ((write_protected_.from <= range.from && range.from <= write_protected_.to) ||
		(write_protected_.from <= range.to && range.to <= write_protected_.to));
}

void elang::memory::manager::deallocate(uint64_type address){
	lock_guard_type lock_guard(lock_);
	return deallocate_(address, true);
}

elang::memory::manager::uint64_type elang::memory::manager::reserve(size_type size){
	lock_guard_type lock_guard(lock_);
	return reserve_(size);
}

elang::memory::manager::block_type *elang::memory::manager::allocate(size_type size, uint64_type address_hint){
	lock_guard_type lock_guard(lock_);
	return allocate_(size, address_hint);
}

elang::memory::manager::block_type *elang::memory::manager::allocate_contiguous(size_type count, size_type size){
	lock_guard_type lock_guard(lock_);
	return allocate_contiguous_(count, size);
}

elang::memory::manager::block_type *elang::memory::manager::reallocate(uint64_type address, size_type size){
	lock_guard_type lock_guard(lock_);
	return reallocate_(address, size);
}

void elang::memory::manager::fill(uint64_type address, char source, size_type count){
	shared_lock_guard_type lock_guard(lock_);
	fill_(address, source, count);
}

void elang::memory::manager::copy(uint64_type destination, uint64_type source, size_type size){
	shared_lock_guard_type lock_guard(lock_);
	copy_(destination, source, size);
}

void elang::memory::manager::write(uint64_type destination, const char *source, size_type size){
	shared_lock_guard_type lock_guard(lock_);
	write_(destination, source, size, true);
}

void elang::memory::manager::write(uint64_type destination, input_reader_type &source, size_type count){
	shared_lock_guard_type lock_guard(lock_);
	write_(destination, source, count);
}

void elang::memory::manager::read(uint64_type source, char *destination, size_type size){
	shared_lock_guard_type lock_guard(lock_);
	read_(source, destination, size);
}

void elang::memory::manager::read(uint64_type source, output_writer_type &destination, size_type count){
	shared_lock_guard_type lock_guard(lock_);
	read_(source, destination, count);
}

elang::memory::manager::block_type *elang::memory::manager::find(uint64_type address){
	shared_lock_guard_type lock_guard(lock_);
	return find_(address);
}

elang::memory::manager::block_type *elang::memory::manager::find_nearest(uint64_type address){
	shared_lock_guard_type lock_guard(lock_);
	return find_nearest_(address);
}

void elang::memory::manager::add_available_(uint64_type value, size_type size){
	if (size == 0u)
		return;

	auto entry = available_.begin();
	for (; entry != available_.end(); ++entry){
		if ((entry->first + entry->second) == value)
			break;//Previous in sequence
	}

	if (entry != available_.end()){//Merge with previous
		entry->second += size;
		value = entry->first;
		size = entry->second;
	}

	auto next = available_.find(value + size);
	if (next != available_.end()){//Merge with next
		entry->second += next->second;
		available_.erase(next);
	}
	else//Add entry
		available_[value] = size;

	if (!available_.empty()){
		auto last = std::prev(available_.end());
		if ((last->first + last->second) == next_address_){//Move next address backwards
			next_address_ = last->first;
			available_.erase(last);
		}
	}
}

elang::memory::manager::uint64_type elang::memory::manager::find_available_(size_type size, uint64_type match){
	for (auto &entry : available_){
		if (match != 0ull && entry.first != match){
			if (match < entry.first)
				break;
			continue;
		}

		if (size < entry.second){//Use required
			available_[entry.first + size] = (entry.second - size);
			return entry.first;
		}

		if (size == entry.second)
			return entry.first;
	}

	return 0ull;
}

void elang::memory::manager::deallocate_(uint64_type address, bool add_to_available){
	auto entry = find_raw_(address);
	if (entry == map_.end())//Invalid address
		throw error_type::invalid_address;

	if (entry->second.ref_count > 0u && --entry->second.ref_count > 0u)
		return;//Block is still referenced

	if (add_to_available)//Add to available list
		add_available_(address, entry->second.actual_size);

	map_.erase(entry);
}

elang::memory::manager::uint64_type elang::memory::manager::reserve_(size_type size){
	if (size == 0u)
		return 0ull;

	auto address = find_available_(size);
	if (address == 0ull){//Use next value
		if ((std::numeric_limits<uint64_type>::max() - size) < address)
			throw error_type::out_of_memory;//Out of address space

		address = next_address_;
		next_address_ += size;
	}
	else//Remove from list
		available_.erase(address);

	return address;
}

elang::memory::manager::block_type *elang::memory::manager::allocate_block_(size_type size, uint64_type address){
	try{
		size_type actual_size = ((size == 0u) ? 1u : size);
		block_type block{
			1u,										//Reference count
			address,								//Address
			size,									//Size
			actual_size,							//Actual size -- 0 for constant block
			std::make_unique<char[]>(actual_size)	//Data
		};

		if (block.data.get() != nullptr)//Add entry
			return &(map_[block.address] = std::move(block));
	}
	catch (...){}

	throw error_type::out_of_memory;
}

elang::memory::manager::block_type *elang::memory::manager::allocate_(size_type size, uint64_type address_hint){
	if (map_.max_size() <= map_.size())
		throw error_type::out_of_memory;//Out of address space

	auto merge = false;
	auto actual_size = ((size == 0u) ? 1u : size);

	if (address_hint == 0ull){//Determine address
		merge = true;
		if ((address_hint = find_available_(actual_size)) == 0ull){//Use next address
			if ((std::numeric_limits<uint64_type>::max() - actual_size) < address_hint)
				throw error_type::out_of_memory;//Out of address space

			address_hint = next_address_;
			next_address_ += actual_size;
		}
		else//Remove from list
			available_.erase(address_hint);
	}
	else if (find_nearest_(address_hint) == nullptr){
		if (next_address_ < (address_hint + actual_size))
			next_address_ = (address_hint + actual_size);
	}
	else
		throw error_type::invalid_address;

	try{
		return allocate_block_(size, address_hint);
	}
	catch (...){
		if (merge)//Return address to available list
			add_available_(address_hint, actual_size);
		throw;//Forward exception
	}

	return nullptr;
}

elang::memory::manager::block_type *elang::memory::manager::allocate_contiguous_(size_type count, size_type size){
	if (count == 0u)
		throw error_type::invalid_count;

	auto actual_size = ((size == 0u) ? 1u : size);
	auto address = reserve_(actual_size * count);

	auto start_value = address;
	std::vector<block_type *> blocks(size);

	for (auto i = 0u; i < count; ++i, address += actual_size){
		try{//Guard allocation
			blocks[i] = allocate_(size, address);
		}
		catch (...){
			for (auto j = 0u; j < i; ++j)//Rollback
				deallocate_(blocks[j]->address, false);

			add_available_(start_value, actual_size * count);
			throw;//Forward exception
		}
	}

	return *blocks.begin();
}

elang::memory::manager::block_type *elang::memory::manager::reallocate_(uint64_type address, size_type size){
	auto block = find_nearest_(address);
	if (block == nullptr)
		throw error_type::invalid_address;

	if (block->size < size){//Expand
		if (block->actual_size < size){//Reallocate
			data_type old_data = std::move(block->data);
			auto old_size = block->actual_size;

			deallocate_(block->address, true);
			block = allocate_(size, 0u);
			memcpy(block->data.get(), old_data.get(), old_size);//Copy data
		}
		else//Expand size
			block->size = size;
	}
	else if (size < block->size)//Shrink
		block->size = size;

	return block;
}

void elang::memory::manager::fill_(uint64_type address, char source, size_type count){
	write_(address, &source, count, false);
}

void elang::memory::manager::copy_(uint64_type destination, uint64_type source, size_type size){
	block_type *block = nullptr;
	size_type available_size = 0u, target_size = 0u, ptr_index = 0u;

	while (size > 0u){
		if ((block = (block == nullptr) ? find_nearest_(source) : find_(source)) != nullptr){//Next block
			ptr_index = static_cast<size_type>(source - block->address);
			available_size = (block->actual_size - ptr_index);
		}
		else//No next block
			throw error_type::invalid_address;

		target_size = (available_size < size) ? available_size : size;
		write_(destination, block->data.get() + ptr_index, target_size, true);

		source += target_size;
		destination += target_size;
		size -= target_size;
	}
}

void elang::memory::manager::write_(uint64_type destination, const char *source, size_type size, bool is_array){
	if (size == 0u)//Do nothing
		return;

	range_type range{ destination, (destination + (size - 1)) };
	if (is_access_protected(range))
		throw error_type::access_violation;

	if (is_write_protected(range))
		throw error_type::write_access_violation;

	block_type *block = nullptr;
	size_type available_size = 0u, min_size = 0u, ptr_index = 0u;

	while (size > 0u){
		if ((block = ((block == nullptr) ? find_nearest_(destination) : find_(destination))) != nullptr){//Valid block
			ptr_index = static_cast<size_type>(destination - block->address);
			available_size = (block->actual_size - ptr_index);
		}
		else//No next block
			throw error_type::invalid_address;

		min_size = (available_size < size) ? available_size : size;
		if (is_array){
			memcpy(block->data.get() + ptr_index, source, min_size);
			source += min_size;//Advance source pointer
		}
		else//Set applicable
			std::memset(block->data.get() + ptr_index, *source, min_size);

		destination += min_size;
		size -= min_size;
	}
}

void elang::memory::manager::write_(uint64_type destination, input_reader_type &source, size_type count){
	if (count == 0u)//Do nothing
		return;

	range_type range{ destination, (destination + (count - 1)) };
	if (is_access_protected(range))
		throw error_type::access_violation;

	if (is_write_protected(range))
		throw error_type::write_access_violation;

	block_type *block = nullptr;
	size_type available_size = 0u, min_size = 0u, ptr_index = 0u;

	while (count > 0u){
		if ((block = ((block == nullptr) ? find_nearest_(destination) : find_(destination))) != nullptr){//Valid block
			ptr_index = static_cast<size_type>(destination - block->address);
			available_size = (block->actual_size - ptr_index);
		}
		else//No next block
			throw error_type::invalid_address;

		min_size = (available_size < count) ? available_size : count;
		source.read(block->data.get() + ptr_index, min_size);

		destination += min_size;
		count -= min_size;
	}
}

void elang::memory::manager::read_(uint64_type source, char *destination, size_type size){
	block_type *block = nullptr;
	size_type available_size = 0u, min_size = 0u, ptr_index = 0u;

	while (size > 0u){
		if ((block = (block == nullptr) ? find_nearest_(source) : find_(source)) != nullptr){//Valid block
			ptr_index = static_cast<size_type>(source - block->address);
			available_size = (block->actual_size - ptr_index);
		}
		else//No next block
			throw error_type::invalid_address;

		min_size = (available_size < size) ? available_size : size;
		memcpy(destination, block->data.get() + ptr_index, min_size);//Read block

		destination += min_size;
		source += min_size;
		size -= min_size;
	}
}

void elang::memory::manager::read_(uint64_type source, output_writer_type &destination, size_type count){
	block_type *block = nullptr;
	size_type available_size = 0u, min_size = 0u, ptr_index = 0u;

	while (count > 0u){
		if ((block = (block == nullptr) ? find_nearest_(source) : find_(source)) != nullptr){//Valid block
			ptr_index = static_cast<size_type>(source - block->address);
			available_size = (block->actual_size - ptr_index);
		}
		else//No next block
			throw error_type::invalid_address;

		min_size = (available_size < count) ? available_size : count;
		destination.write(block->data.get() + ptr_index, min_size);//Read block

		source += min_size;
		count -= min_size;
	}
}

elang::memory::manager::map_iterator_type elang::memory::manager::find_raw_(uint64_type address){
	if (is_access_protected(range_type{ address, address }))
		throw error_type::access_violation;
	return map_.find(address);
}

elang::memory::manager::block_type *elang::memory::manager::find_(uint64_type address){
	auto entry = find_raw_(address);
	return ((entry == map_.end()) ? nullptr : &entry->second);
}

elang::memory::manager::block_type *elang::memory::manager::find_nearest_(uint64_type address){
	if (is_access_protected(range_type{ address, address }))
		throw error_type::access_violation;

	block_type *block = nullptr;
	for (auto &entry : map_){
		if (entry.first == address || (entry.first < address && address < (entry.first + entry.second.actual_size))){
			block = &entry.second;
			break;
		}
	}

	return block;
}
