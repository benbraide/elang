#pragma once

#ifndef ELANG_INSTRUCTION_SECTION_H
#define ELANG_INSTRUCTION_SECTION_H

#include <variant>
#include <unordered_map>

#include "../vm/runtime_type.h"

#include "instruction/times_instruction.h"

#include "instruction_label.h"

namespace elang::easm{
	enum class section_id{
		rodata,
		data,
		text,
		type,
		meta,
	};

	class instruction_section_base{
	public:
		typedef unsigned __int64 uint64_type;
		typedef std::size_t size_type;

		typedef section_id id_type;
		typedef instruction_error error_type;

		typedef instruction::base instruction_type;
		typedef instruction_type::ptr_type instruction_ptr_type;

		typedef elang::vm::runtime_type::ptr_type runtime_ptr_type;
		typedef elang::common::output_writer writer_type;

		typedef std::vector<std::string> string_list_type;

		struct printer{
			explicit printer(writer_type &writer)
				: writer_(&writer){}

			void operator ()(const instruction_type *instruction) const{
				writer_->write("  ");
				instruction->print(*writer_);
			}

			void operator ()(const instruction_label::ptr_type label) const{
				label->print(*writer_);
			}

			void operator ()(const std::string &start_label) const{
				writer_->write("  .start_label ").write(start_label).write(writer_type::manip_type::newline);
			}

			void operator ()(size_type stack_size) const{
				writer_->write("  .stack_size ").write(stack_size).write(writer_type::manip_type::newline);
			}

		private:
			writer_type *writer_;
		};

		explicit instruction_section_base(id_type id);

		virtual id_type id() const;

		virtual void print(writer_type &writer) const;

		virtual void write_memory() const = 0;

		virtual void set_seg_offset(uint64_type value);

		virtual uint64_type get_seg_offset() const;

		virtual uint64_type get_offset() const;

		virtual instruction_label *add(instruction_label *parent, const std::string &label);

		virtual void add(instruction_ptr_type instruction);

		virtual void add(runtime_ptr_type type);

		virtual void add(const std::string &start_label);

		virtual void add(size_type stack_size);

		virtual instruction_type *find(uint64_type offset, bool is_relative) const;

		virtual instruction_label *find(const std::string &first, const std::vector<std::string> &rest) const;

		virtual uint64_type find(const instruction_label &label) const;

		virtual uint64_type find_address(const std::string &first, const std::vector<std::string> &rest) const;

	protected:
		virtual void print_content_(writer_type &writer) const;

		virtual instruction_type *find_(uint64_type offset) const;

		id_type id_;
		uint64_type offset_;
		uint64_type seg_offset_;
	};

	class instruction_section : public instruction_section_base{
	public:
		using instruction_section_base::add;
		using instruction_section_base::find;

		typedef std::variant<instruction_type *, instruction_label::ptr_type> variant_type;
		typedef std::list<variant_type> order_list_type;

		typedef std::unordered_map<uint64_type, instruction_ptr_type> instruction_list_type;
		typedef std::unordered_map<instruction_label *, uint64_type> label_list_type;

		explicit instruction_section(id_type id);

		virtual void write_memory() const override;

		virtual instruction_label *add(instruction_label *parent, const std::string &label) override;

		virtual void add(instruction_ptr_type instruction) override;

		virtual instruction_label *find(const std::string &first, const std::vector<std::string> &rest) const override;

		virtual uint64_type find(const instruction_label &label) const override;

		virtual uint64_type find_address(const std::string &first, const std::vector<std::string> &rest) const;

	protected:
		virtual void print_content_(writer_type &writer) const override;

		virtual instruction_type *find_(uint64_type offset) const override;

		order_list_type order_list_;
		instruction_list_type instruction_list_;
		label_list_type label_list_;
	};
}

#endif /* !ELANG_INSTRUCTION_SECTION_H */
