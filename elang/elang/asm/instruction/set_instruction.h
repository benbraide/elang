#pragma once

#ifndef ELANG_SET_INSTRUCTION_H
#define ELANG_SET_INSTRUCTION_H

#include "../../vm/machine.h"

#include "instruction_base.h"

namespace elang::easm::instruction{
	enum class set_instruction_id{
		setz,
		setnz,
		sete,
		setne,
		setl,
		setnl,
		setle,
		setnle,
		setg,
		setng,
		setge,
		setnge,
	};

	enum class set_instruction_cmp_target{
		less,
		less_or_equal,
		equal,
		greater_or_equal,
		greater,
	};

	template <set_instruction_id id, set_instruction_cmp_target cmp_target, bool invert>
	class set : public base{
	public:
		template <typename... args_types>
		explicit set(args_types &&... args)
			: base("", std::forward<args_types>(args)...){
			switch (id){
			case set_instruction_id::setz:
				name_ = "setz";
				break;
			case set_instruction_id::setnz:
				name_ = "setnz";
				break;
			case set_instruction_id::sete:
				name_ = "sete";
				break;
			case set_instruction_id::setne:
				name_ = "setne";
				break;
			case set_instruction_id::setl:
				name_ = "setl";
				break;
			case set_instruction_id::setnl:
				name_ = "setnl";
				break;
			case set_instruction_id::setle:
				name_ = "setle";
				break;
			case set_instruction_id::setnle:
				name_ = "setnle";
				break;
			case set_instruction_id::setg:
				name_ = "setg";
				break;
			case set_instruction_id::setng:
				name_ = "setng";
				break;
			case set_instruction_id::setge:
				name_ = "setge";
				break;
			case set_instruction_id::setnge:
				name_ = "setnge";
				break;
			default:
				break;
			}
		}

		virtual void validate_operands() const override{
			if (operands_.size() != 1u)
				throw error_type::bad_operand_count;

			if (operands_[0]->id() != operand_id_type::register_value && operands_[0]->value_type() != value_type_id_type::byte)
				throw error_type::bad_operand;
		}

		virtual void execute() const override{
			if (test_())//Passed
				operands_[0]->write(static_cast<__int8>(1));
			else//Failed
				operands_[0]->write(static_cast<__int8>(0));
		}

	protected:
		virtual bool test_() const{
			typedef set_instruction_cmp_target setict;

			auto cmp_state = elang::vm::machine::cached_registers.compare_target->read<__int64>();
			if (cmp_state < 0)//Less
				return ((cmp_target == setict::less || cmp_target == setict::less_or_equal) != invert);

			if (cmp_state == 0)//Equal
				return ((cmp_target == setict::less_or_equal || cmp_target == setict::equal || cmp_target == setict::greater_or_equal) != invert);

			return ((cmp_target == setict::greater || cmp_target == setict::greater_or_equal) != invert);
		}
	};

	using setz		= set<set_instruction_id::setz, set_instruction_cmp_target::equal, false>;
	using setnz		= set<set_instruction_id::setnz, set_instruction_cmp_target::equal, true>;

	using sete		= set<set_instruction_id::sete, set_instruction_cmp_target::equal, false>;
	using setne		= set<set_instruction_id::setne, set_instruction_cmp_target::equal, true>;

	using setl		= set<set_instruction_id::setl, set_instruction_cmp_target::less, false>;
	using setnl		= set<set_instruction_id::setnl, set_instruction_cmp_target::less, true>;

	using setle		= set<set_instruction_id::setle, set_instruction_cmp_target::less_or_equal, false>;
	using setnle	= set<set_instruction_id::setnle, set_instruction_cmp_target::less_or_equal, true>;

	using setg		= set<set_instruction_id::setg, set_instruction_cmp_target::greater, false>;
	using setng		= set<set_instruction_id::setng, set_instruction_cmp_target::greater, true>;

	using setge		= set<set_instruction_id::setge, set_instruction_cmp_target::greater_or_equal, false>;
	using setnge	= set<set_instruction_id::setnge, set_instruction_cmp_target::greater_or_equal, true>;
}

#endif /* !ELANG_SET_INSTRUCTION_H */
