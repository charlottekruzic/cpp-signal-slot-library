#ifndef SIGNAL_H
#define SIGNAL_H

#include <optional>
#include <functional>
#include <unordered_map>
#include <vector>


namespace sig
{
	/*******************************************************************************
	 *                               DiscardCombiner
	 *******************************************************************************/

	class DiscardCombiner
	{
	public:
		using result_type = void;

		template <typename U>
		void combine(U item)
		{
			// do nothing
		}

		result_type result()
		{
			return result_type();// do nothing
		}
	};

	/*******************************************************************************
	 *                               LastCombiner
	 *******************************************************************************/

	template <typename T>
	class LastCombiner
	{
	public:
		using result_type = std::optional<T>;

		template <typename U>
		void combine(U item)
		{
			m_lastResult = item;
		}

		result_type result()
		{
			return m_lastResult;
		}

	private:
		result_type m_lastResult;
	};

	/*******************************************************************************
	 *                               VectorCombiner
	 *******************************************************************************/

	template <typename T>
	class VectorCombiner
	{
	public:
		using result_type = std::vector<T>;

		template <typename U>
		void combine(U &&item)
		{
			m_all_results.emplace_back(std::forward<U>(item));
		}

		result_type result()
		{
			return m_all_results;
		}

	private:
		result_type m_all_results;
	};

	/*******************************************************************************
	 *                               Signal
	 *******************************************************************************/

	template <typename Signature, typename Combiner = DiscardCombiner>
	class Signal;

	template <typename R, typename... Args, typename Combiner>
	class Signal<R(Args...), Combiner>
	{

	public:
		using combiner_type = Combiner;
		using result_type = typename Combiner::result_type;
		using signature_type = R(Args...);

		Signal(Combiner combiner = Combiner())
			: m_combiner(combiner), m_next_id(0)
		{
		}
		
		std::size_t connectSlot(std::function<signature_type> callback)
		{
			std::size_t id = m_next_id;
			m_slots.emplace(id, callback);
			/*m_slots[id] = callback;*/
			m_next_id++;
			return id;
		}

		void disconnectSlot(std::size_t id)
		{
			m_slots.erase(id);
		}
		
		template <typename U = Combiner>
		std::enable_if_t<!std::is_same_v<U, DiscardCombiner>, typename Combiner::result_type>
		emitSignal(Args... args)
		{
			typename Combiner::result_type result = m_combiner.result();
			for (auto& slot : m_slots)
			{
				result = m_combiner.combine(result, slot.second(args...));
			}
			return result;
		}

		template <typename U = Combiner>
		std::enable_if_t<std::is_same_v<U, DiscardCombiner>, void>
		emitSignal(Args... args)
		{
			for (auto& slot : m_slots)
			{
				slot.second(args...);
			}
		}

	private:
		combiner_type m_combiner;
		std::unordered_map<std::size_t, std::function<signature_type>> m_slots;
		std::size_t m_next_id;
	};

}

#endif // SIGNAL_H
