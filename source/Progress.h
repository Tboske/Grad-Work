#pragma once
#include <chrono>

using namespace std::chrono;

class Progress
{
public:
	~Progress() {};
	Progress(Progress&&) = delete;
	Progress(const Progress&) = delete;
	Progress& operator=(Progress&&) = delete;
	Progress& operator=(const Progress&) = delete;

	static void LoadingPopUp()									{ GetInstance()->LoadingPopUpImpl(); }
	static void Start(const std::string& title, const std::string& desc = "", float maxVal = 1.f)
																{ GetInstance()->StartImpl(title, desc, maxVal); }
	static void Reset(const std::string& desc, float maxVal)	{ GetInstance()->ResetImpl(desc, maxVal); }
	static void End()											{ GetInstance()->m_Active = false; }
	static void SetValue(float value)							{ GetInstance()->SetValueImpl(value); }


private:
	Progress() = default;
	static Progress* GetInstance()
	{
		static Progress* pInstance{ new Progress() };
		return pInstance;
	}
	bool m_Active = false;
	float m_Value = 0.f;
	float m_MaxValue = 1.f;
	std::string m_Title;
	std::string m_Description;
	steady_clock::time_point m_StartTime;	

	void LoadingPopUpImpl() const;
	void StartImpl(const std::string& title, const std::string& desc, float maxVal);
	void ResetImpl(const std::string& desc, float maxVal);
	void SetValueImpl(float value);
};

