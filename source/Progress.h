#pragma once
#include <chrono>

using namespace std::chrono;

class Progress
{
public:
	~Progress() {};
	static void LoadingPopUp() { GetInstance()->LoadingPopUpImpl(); }

	static void Start(const std::string& s, float maxVal = 1.f) { GetInstance()->StartImpl(s, maxVal); }
	static void ResetProgress(float maxValue, const std::string& s = "<Unspecified>") { GetInstance()->ResetProgressImpl(maxValue, s); }
	static void SetInactive() { GetInstance()->m_Active = 0.f; }
	static void SetValue(float value) { GetInstance()->SetValueImpl(value); }


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
	std::string m_Description;
	steady_clock::time_point m_StartTime;	



	void LoadingPopUpImpl() const;
	void StartImpl(const std::string& s, float maxVal);
	void ResetProgressImpl(float maxVal, const std::string& s);
	void SetValueImpl(float value);
};

