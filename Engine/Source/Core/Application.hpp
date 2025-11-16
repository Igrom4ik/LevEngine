#pragma once

namespace LEN
{
	class Application {
	public:
		virtual bool Init() = 0;

		// Deltatime in seconds
		virtual void Update(float deltatime) = 0;
		
		virtual void Destroy() = 0;

		void SetNeedsToClose(bool value);
		bool NeedsToBeClose() const;

	private:
		bool m_NeedsToBeClose = false;

	};
}




