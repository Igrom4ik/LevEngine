//
// Created by igrom on 16.11.2025.
//

#include "Application.hpp"

namespace LEN
{
	void Application::SetNeedsToClose(bool value)
	{
		m_NeedsToBeClose = value;
	}

	bool Application::NeedsToBeClose() const
	{
		return m_NeedsToBeClose;
	}
}


