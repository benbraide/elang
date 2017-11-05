#pragma once

#ifndef ELANG_LABEL_INSTRUCTION_OPERAND_H
#define ELANG_LABEL_INSTRUCTION_OPERAND_H

#include "../../vm/machine.h"

#include "../instruction_label.h"

#include "constant_value_instruction_operand.h"

namespace elang::easm::instruction{
	class label_operand : public constant_value_operand<unsigned __int64>{
	public:
		typedef constant_value_operand<unsigned __int64> base_type;

		typedef std::vector<std::string> string_list_type;

		label_operand(const std::string &first, const string_list_type &rest)
			: base_type(value_type_id_type::qword), first_(first), rest_(rest), label_(nullptr){
			init_();
		}

		label_operand(std::string &&first, string_list_type &&rest)
			: base_type(value_type_id_type::qword), first_(std::move(first)), rest_(std::move(rest)), label_(nullptr){
			init_();
		}

		virtual id_type id() const override{
			return id_type::label;
		}

		virtual uint64_type read_64bits() const override{
			return 0;
		}

		virtual void write_to_memory(char *buffer) const override{
			throw error_type::bad_operation;
		}

		virtual void print(writer_type &writer, writer_type &wide_writer) const override{
			if (!first_.empty())
				writer << first_;

			for (auto &entry : rest_)
				writer << "." << entry;

			writer << writer_type::manip_type::flush;
		}

		virtual void update_constant_value_type(value_type_id_type id) override{}

		virtual void resolve(elang::vm::asm_translation &translation){
			if (label_ == nullptr && (value_ = translation.find_address(first_, rest_)) == static_cast<uint64_type>(-1))
				throw error_type::label_not_found;
			else if (label_ != nullptr)//Get value
				value_ = translation.find(*label_);
		}

	protected:
		void init_(){
			if (elang::vm::machine::asm_translation.active_label() != nullptr)
				label_ = elang::vm::machine::asm_translation.active_label()->find(first_, rest_);

			if (label_ == nullptr)//Schedule resolving
				elang::vm::machine::asm_translation.add(*this);
		}

		std::string first_;
		string_list_type rest_;
		instruction_label *label_;
	};
}

#endif /* !ELANG_LABEL_INSTRUCTION_OPERAND_H */
