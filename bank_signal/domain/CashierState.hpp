#pragma once
enum class CashierState {
	Offline,//server-side
	Ready,//при первом подключении, нажатие на кнопку даст о себе знать
	Busy,
	Free//свободная касса
};