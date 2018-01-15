﻿#pragma once

#include <wrl.h>
#include <cstdint>
#include <exception>


namespace DX
{
	// アニメーションとシミュレーションのタイミング用のヘルパー クラス。
	class StepTimer
	{
	public:
		StepTimer() : 
			m_elapsedTicks(0),
			m_totalTicks(0),
			m_leftOverTicks(0),
			m_frameCount(0),
			m_framesPerSecond(0),
			m_framesThisSecond(0),
			m_qpcSecondCounter(0),
			m_isFixedTimeStep(false),
			m_targetElapsedTicks(TicksPerSecond / 60)
		{
			if (!QueryPerformanceFrequency(&m_qpcFrequency))
			{
				throw std::exception();
			}

			if (!QueryPerformanceCounter(&m_qpcLastTime))
			{
				throw std::exception();
			}

			// 最大デルタを 1 秒の 1/10 に初期化します。
			m_qpcMaxDelta = m_qpcFrequency.QuadPart / 10;
		}

		// 前の Update 呼び出しから経過した時間を取得します。
		std::uint64_t GetElapsedTicks() const						{ return m_elapsedTicks; }
		double GetElapsedSeconds() const					{ return TicksToSeconds(m_elapsedTicks); }

		// プログラム開始から経過した合計時間を取得します。
		std::uint64_t GetTotalTicks() const						{ return m_totalTicks; }
		double GetTotalSeconds() const						{ return TicksToSeconds(m_totalTicks); }

		// プログラム開始からの合計更新回数を取得します。
		std::uint32_t GetFrameCount() const						{ return m_frameCount; }

		// 現在のフレーム レートを取得します。
		std::uint32_t GetFramesPerSecond() const					{ return m_framesPerSecond; }

		// 固定または可変のどちらのタイムステップ モードを使用するかを設定します。
		void SetFixedTimeStep(bool isFixedTimestep)			{ m_isFixedTimeStep = isFixedTimestep; }

		// 固定タイムステップ モードでは、Update の呼び出し頻度を設定します。
		void SetTargetElapsedTicks(std::uint64_t targetElapsed)	{ m_targetElapsedTicks = targetElapsed; }
		void SetTargetElapsedSeconds(double targetElapsed)	{ m_targetElapsedTicks = SecondsToTicks(targetElapsed); }

		// 整数形式は 1 秒あたり 10,000,000 ティックを使用して時間を表します。
		static const std::uint64_t TicksPerSecond = 10000000;

		static double TicksToSeconds(std::uint64_t ticks)			{ return static_cast<double>(ticks) / TicksPerSecond; }
		static std::uint64_t SecondsToTicks(double seconds)		{ return static_cast<std::uint64_t>(seconds * TicksPerSecond); }

		// 意図的なタイミングの不連続性の後 (IO のブロック操作など)
		//これを呼び出すと、固定タイムステップ ロジックによって一連のキャッチアップが試行されるのを回避できます
		//呼び出しの更新。

		void ResetElapsedTime()
		{
			if (!QueryPerformanceCounter(&m_qpcLastTime))
			{
				throw std::exception();
			}

			m_leftOverTicks = 0;
			m_framesPerSecond = 0;
			m_framesThisSecond = 0;
			m_qpcSecondCounter = 0;
		}

		// タイマー状態を更新し、指定の Update 関数を適切な回数だけ呼び出します。
		template<typename TUpdate>
		void Tick(const TUpdate& update)
		{
			// 現在の時刻をクエリします。
			LARGE_INTEGER currentTime;

			if (!QueryPerformanceCounter(&currentTime))
			{
				//throw ref new Platform::FailureException();
                throw std::exception();
			}

			std::uint64_t timeDelta = currentTime.QuadPart - m_qpcLastTime.QuadPart;

			m_qpcLastTime = currentTime;
			m_qpcSecondCounter += timeDelta;

			//極端に大きな時間差 (デバッガーで一時停止した後など) をクランプします。
			if (timeDelta > m_qpcMaxDelta)
			{
				timeDelta = m_qpcMaxDelta;
			}

			// QPC 単位を標準の目盛り形式に変換します。前にクランプが発生しているため、この変換ではオーバーフローが不可能です。
			timeDelta *= TicksPerSecond;
			timeDelta /= m_qpcFrequency.QuadPart;

			std::uint32_t lastFrameCount = m_frameCount;

			if (m_isFixedTimeStep)
			{
				// 固定タイムステップ更新ロジック

				//アプリケーションの実行がターゲット経過時間 (1/4 ミリ秒以内) に非常に近い場合は、
				// ターゲット値と正確に一致するようにクロックをクランプしてください。これにより、関係のない小さなエラーが、
				//時間の経過とともに蓄積されていくことを防止できます。このクランプを実行しないと、60 fps の
				//固定の更新を要求したゲーム (59.94 NTSC ディスプレイ上で有効な vsync で実行) は、
				//小さなエラーが蓄積して最終的にはフレームをドロップすることになります。スムーズに実行できるように、わずかな偏差は
				//ゼロに丸めることをお勧めします。

				if (abs(static_cast<std::int64_t>(timeDelta - m_targetElapsedTicks)) < TicksPerSecond / 4000)
				{
					timeDelta = m_targetElapsedTicks;
				}

				m_leftOverTicks += timeDelta;

				while (m_leftOverTicks >= m_targetElapsedTicks)
				{
					m_elapsedTicks = m_targetElapsedTicks;
					m_totalTicks += m_targetElapsedTicks;
					m_leftOverTicks -= m_targetElapsedTicks;
					m_frameCount++;

					update();
				}
			}
			else
			{
				// 可変タイムステップ更新ロジック。
				m_elapsedTicks = timeDelta;
				m_totalTicks += timeDelta;
				m_leftOverTicks = 0;
				m_frameCount++;

				update();
			}

			// 現在のフレーム レートを追跡します。
			if (m_frameCount != lastFrameCount)
			{
				m_framesThisSecond++;
			}

			if (m_qpcSecondCounter >= static_cast<std::uint64_t>(m_qpcFrequency.QuadPart))
			{
				m_framesPerSecond = m_framesThisSecond;
				m_framesThisSecond = 0;
				m_qpcSecondCounter %= m_qpcFrequency.QuadPart;
			}
		}

	private:
		// ソース タイミング データでは QPC 単位を使用します。
		LARGE_INTEGER m_qpcFrequency;
		LARGE_INTEGER m_qpcLastTime;
		std::uint64_t m_qpcMaxDelta;

		// 派生タイミング データでは、標準の目盛り形式を使用します。
		std::uint64_t m_elapsedTicks;
		std::uint64_t m_totalTicks;
		std::uint64_t m_leftOverTicks;

		// フレーム レートの追跡用メンバー。
		std::uint32_t m_frameCount;
		std::uint32_t m_framesPerSecond;
		std::uint32_t m_framesThisSecond;
		std::uint64_t m_qpcSecondCounter;

		// 固定タイムステップ モードの構成用メンバー。
		bool m_isFixedTimeStep;
		std::uint64_t m_targetElapsedTicks;
	};
}
