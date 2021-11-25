#pragma once
#include <chrono>

using namespace std::chrono;

class Progress
{
public:
	~Progress();
	static void LoadingPopUp() { GetInstance()->LoadingPopUpImpl(); }

	static void Start(const std::string& s) { GetInstance()->StartImpl(s); }
	static void ResetProgress(const std::string& s = "<Unspecified>") { GetInstance()->ResetProgressImpl(s); }
	static void SetInactive() { GetInstance()->m_Active = 0.f; }
	static void SetValue(float value) { GetInstance()->m_Value = value; }

private:
	Progress() = default;
	static Progress* GetInstance()
	{
		static Progress* pInstance{ new Progress() };
		return pInstance;
	}

	void LoadingPopUpImpl() const;
	void StartImpl(const std::string& s);
	void ResetProgressImpl(const std::string& s);

	bool m_Active = false;
	float m_Value = 0.f;
	std::string m_Description;
	steady_clock::time_point m_StartTime;	
};

