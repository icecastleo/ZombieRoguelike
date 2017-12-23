#pragma once
#include <vector>
#include <functional>
#include <utility>
#include <algorithm>

namespace bt1
{
	enum Status
		/**
		* Return values of and valid states for behaviors.
		*/
	{
		BH_INVALID,
		BH_SUCCESS,
		BH_FAILURE,
		BH_RUNNING,
		BH_ABORTED,
	};

	class Behavior
		/**
		* Base class for actions, conditions and composites.
		*/
	{
	public:
		virtual Status update() = 0;

		virtual void onInitialize() {}
		virtual void onTerminate(Status) {}
		virtual float calculateUtility() { return 0.0f; };

		Behavior()
			: m_eStatus(BH_INVALID)
		{
		}

		virtual ~Behavior()
		{
		}

		Status tick()
		{
			if (m_eStatus != BH_RUNNING)
			{
				onInitialize();
			}

			m_eStatus = update();

			if (m_eStatus != BH_RUNNING)
			{
				onTerminate(m_eStatus);
			}
			return m_eStatus;
		}

		void reset()
		{
			m_eStatus = BH_INVALID;
		}

		void abort()
		{
			onTerminate(BH_ABORTED);
			m_eStatus = BH_ABORTED;
		}

		bool isTerminated() const
		{
			return m_eStatus == BH_SUCCESS || m_eStatus == BH_FAILURE;
		}

		bool isRunning() const
		{
			return m_eStatus == BH_RUNNING;
		}

		Status getStatus() const
		{
			return m_eStatus;
		}

	private:
		Status m_eStatus;
	};

	class Decorator : public Behavior
	{
	protected:
		Behavior* m_pChild;

	public:
		Decorator(Behavior* child) : m_pChild(child) {}
	};

	class Repeat : public Decorator
	{
	public:
		Repeat(Behavior* child)
			: Decorator(child)
		{
		}

		void setCount(int count)
		{
			m_iLimit = count;
		}

		void onInitialize()
		{
			m_iCounter = 0;
		}

		Status update()
		{
			for (;;)
			{
				m_pChild->tick();
				if (m_pChild->getStatus() == BH_RUNNING) break;
				if (m_pChild->getStatus() == BH_FAILURE) return BH_FAILURE;
				if (++m_iCounter == m_iLimit) return BH_SUCCESS;
				m_pChild->reset();
			}
			return BH_INVALID;
		}

	protected:
		int m_iLimit;
		int m_iCounter;
	};

	class Composite : public Behavior
	{
	public:
		virtual ~Composite() {
			for (Behavior *beh : m_Children) {
				delete beh;
			}
		}

		void addChild(Behavior* child) { m_Children.push_back(child); }
		void removeChild(Behavior*);
		void clearChildren();
	protected:
		typedef std::vector<Behavior*> Behaviors;
		Behaviors m_Children;
	};

	class Sequence : public Composite
	{
	protected:
		virtual ~Sequence()
		{
		}

		virtual void onInitialize()
		{
			m_CurrentChild = m_Children.begin();
		}

		virtual Status update()
		{
			// Keep going until a child behavior says it's running.
			for (;;)
			{
				Status s = (*m_CurrentChild)->tick();

				// If the child fails, or keeps running, do the same.
				if (s != BH_SUCCESS)
				{
					return s;
				}

				// Hit the end of the array, job done!
				if (++m_CurrentChild == m_Children.end())
				{
					return BH_SUCCESS;
				}
			}
		}

		Behaviors::iterator m_CurrentChild;
	};

	class Selector : public Composite
	{
	protected:
		virtual ~Selector()
		{
		}

		virtual void onInitialize()
		{
			m_Current = m_Children.begin();
		}

		virtual Status update()
		{
			// Keep going until a child behavior says its running.
			for (;;)
			{
				Status s = (*m_Current)->tick();

				// If the child succeeds, or keeps running, do the same.
				if (s != BH_FAILURE)
				{
					return s;
				}

				// Hit the end of the array, it didn't end well...
				if (++m_Current == m_Children.end())
				{
					return BH_FAILURE;
				}
			}
		}

		Behaviors::iterator m_Current;
	};

	class Parallel : public Composite
	{
	public:
		enum Policy
		{
			RequireOne,
			RequireAll,
		};

		Parallel(Policy forSuccess, Policy forFailure)
			: m_eSuccessPolicy(forSuccess)
			, m_eFailurePolicy(forFailure)
		{
		}

		virtual ~Parallel() {}

	protected:
		Policy m_eSuccessPolicy;
		Policy m_eFailurePolicy;

		virtual Status update()
		{
			size_t iSuccessCount = 0, iFailureCount = 0;

			for (Behaviors::iterator it = m_Children.begin(); it != m_Children.end(); ++it)
			{
				Behavior& b = **it;
				if (!b.isTerminated())
				{
					b.tick();
				}

				if (b.getStatus() == BH_SUCCESS)
				{
					++iSuccessCount;
					if (m_eSuccessPolicy == RequireOne)
					{
						return BH_SUCCESS;
					}
				}

				if (b.getStatus() == BH_FAILURE)
				{
					++iFailureCount;
					if (m_eFailurePolicy == RequireOne)
					{
						return BH_FAILURE;
					}
				}
			}

			if (m_eFailurePolicy == RequireAll  &&  iFailureCount == m_Children.size())
			{
				return BH_FAILURE;
			}

			if (m_eSuccessPolicy == RequireAll  &&  iSuccessCount == m_Children.size())
			{
				return BH_SUCCESS;
			}

			return BH_RUNNING;
		}

		virtual void onTerminate(Status)
		{
			for (Behaviors::iterator it = m_Children.begin(); it != m_Children.end(); ++it)
			{
				Behavior& b = **it;
				if (b.isRunning())
				{
					b.abort();
				}
			}
		}
	};

	class Monitor : public Parallel
	{
	public:
		Monitor()
			: Parallel(Parallel::RequireOne, Parallel::RequireOne)
		{
		}

		void addCondition(Behavior* condition)
		{
			m_Children.insert(m_Children.begin(), condition);
		}

		void addAction(Behavior* action)
		{
			m_Children.push_back(action);
		}
	};

	class ActiveSelector : public Selector
	{
	protected:

		virtual void onInitialize()
		{
			m_Current = m_Children.end();
		}

		virtual Status update()
		{
			Behaviors::iterator previous = m_Current;

			Selector::onInitialize();
			Status result = Selector::update();

			if (previous != m_Children.end() && m_Current != previous)
			{
				(*previous)->onTerminate(BH_ABORTED);
			}
			return result;
		}
	};

	// lambda condition
	class BoolCondition : public Behavior 
	{
	public:
		BoolCondition(std::function<bool()> condition)
			:condition(condition) {}

	protected:
		std::function<bool()> condition;

		virtual Status update()
		{
			if (condition()) {
				return BH_SUCCESS;
			}
			else {
				return BH_FAILURE;
			}
		}
	};

	class Wait : public Behavior {

	public:
		Wait(float millisecond)
			:millisecond(millisecond) {}

	protected:
		float millisecond;
		float counter;

		virtual void onInitialize() {
			counter = 0;
		}

		virtual Status update()
		{
			counter += MS_PER_UPDATE;

			if (counter >= millisecond) {
				return BH_SUCCESS;
			}
			else {
				return BH_RUNNING;
			}
		}
	};

	// lambda action
	class SimpleAction : public Behavior {
	public:
		SimpleAction(std::function<void()> action)
			:action(action) {}

	protected:
		std::function<void()> action;

		virtual Status update()
		{
			action();
			return BH_SUCCESS;
		}
	};

	typedef std::pair<Behavior *, float> PAIR;

	class UtilitySelector : public Composite {
	
	protected:		
		std::vector<PAIR> utility;

		virtual Status update()
		{
			if (utility.size() == 0)
			{
				//query for child utility values.
				for (auto m_Current = m_Children.begin(); m_Current != m_Children.end(); m_Current++)
				{
					utility.push_back(std::make_pair(*m_Current, (*m_Current)->calculateUtility()));
				}
			}
			
			//sort from highest utility to lowest.
			sort(utility.begin(), utility.end(), [](const PAIR& lhs, const PAIR& rhs) {
				return lhs.second > rhs.second;
			});
			
			auto m_Current = utility.begin();
			
			//evaluate in utility order and select the first valid child.
			while (m_Current != utility.end())
			{
				Status s = (*m_Current).first->update();

				if (s == BH_RUNNING) {
					return BH_RUNNING;
				}
				else if (s == BH_SUCCESS)
				{
					utility.clear();
					return BH_SUCCESS;
				}
				m_Current++;
			}

			utility.clear();
			return BH_FAILURE;
		}
	};

} // namespace bt1