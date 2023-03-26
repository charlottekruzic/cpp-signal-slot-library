#ifndef SIGNAL_H
#define SIGNAL_H

#include <functional>
#include <map>
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
			// do nothing
		}
	};

	/*******************************************************************************
	 *                               LastCombiner
	 *******************************************************************************/

	template <typename T>
	class LastCombinerBase
	{
		template <typename U>
		void combine(U item)
		{
			// do nothing
		}

		void result()
		{
			// do nothing
		}
	};

	template <typename T>
	class LastCombiner : public LastCombinerBase<T>
	{
	public:
		using result_type = T;

		template <typename U>
		void combine(U item)
		{
			m_lastResult = std::forward<U>(item);
		}

		result_type result()
		{
			return std::move(m_lastResult);
		}

	private:
		result_type m_lastResult;
	};

	template <>
	class LastCombiner<void> : public LastCombinerBase<void>
	{
	public:
		using result_type = void;
	};

	/*******************************************************************************
	 *                               VectorCombiner
	 *******************************************************************************/
	template <typename T>
	class VectorCombinerBase
	{
		template <typename U>
		void combine(U item)
		{
			// do nothing
		}

		void result()
		{
			// do nothing
		}
	};

	template <typename T>
	class VectorCombiner : public VectorCombinerBase<T>
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
			return std::move(m_all_results);
		}

	private:
		result_type m_all_results;
	};

	// type void
	template <>
	class VectorCombiner<void> : public VectorCombinerBase<void>
	{
	public:
		using result_type = void;
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
			: m_combiner(std::move(combiner)), m_id(0)
		{
		}

		std::size_t connectSlot(std::function<signature_type> callback)
		{
			std::size_t id = m_id;
			m_slots.emplace(id, callback);
			m_id++;
			return id;
		}

		void disconnectSlot(std::size_t id)
		{
			m_slots.erase(id);
		}

		result_type emitSignal(Args... args)
		{
			if constexpr (std::is_void_v<result_type>)
			{
				for (auto &slot : m_slots)
				{
					slot.second(std::forward<Args>(args)...);
				}
			}
			else
			{
				for (auto &slot : m_slots)
				{
					m_combiner.combine(slot.second(std::forward<Args>(args)...));
				}
				return m_combiner.result();
			}
		}

	private:
		combiner_type m_combiner;
		std::map<std::size_t, std::function<signature_type>> m_slots;
		std::size_t m_id;
	};

}

#endif // SIGNAL_H
