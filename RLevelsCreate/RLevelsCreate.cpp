#include "pch.h"
#include "ScriptInterfaceUnit.h"

#include <format>

using namespace std::string_literals;

static const PChar scriptShortName = "RLevelsCreate";
static const PChar scriptDescription = "Draws R levels for the first order found";

static int maxRLevels = 7;
static int timeframe = 15;


EXPORT void __stdcall Init()
{
	Print("INFO: EXPORT void __stdcall Init()");

	ScriptShortName(scriptShortName);
	ScriptDescription(scriptDescription);

	RegOption("MaxRLevels", ot_Integer, &maxRLevels);
	SetOptionRange("MaxRLevels", 1, 10);
}

EXPORT void __stdcall Done()
{
	Print("INFO: EXPORT void __stdcall Done()");
}

EXPORT void __stdcall Execute()
{
	Print(
		std::format("INFO: EXPORT void __stdcall Execute(): maxRLevels: {}, timeframe: '{}'", maxRLevels, timeframe)
	);
	
	const int ordersTotal = OrdersTotal();

	Print(
		std::format("INFO: Execute(): ordersTotal={}", ordersTotal)
	);

	if (0 == ordersTotal)
	{
		Print(
			std::format("INFO: No order found. ordersTotal={}", ordersTotal)
		);
		return;
	}

	if(OrderSelect(0, SELECT_BY_POS, MODE_TRADES))
	{
		Print(
			std::format("INFO: Execute(): Order selected")
		);

		const double slPrice = OrderStopLoss();

		if(slPrice == 0.0)
		{
			Print(
				std::format("INFO: Order's StopLoss not set.")
			);
			return;
		}

		const double openPrice = OrderOpenPrice();
		const double oneR = openPrice - slPrice;

		const int orderTicket = OrderTicket();

		const char* orderSymbol = OrderSymbol();

		Print(
			std::format("INFO: Execute(): openPrice={}, slPrice={}, oneR={}, orderTicket={}, orderSymbol='{}'", openPrice, slPrice, oneR, orderTicket, orderSymbol)
		);

		if(SetCurrencyAndTimeframe(orderSymbol, timeframe))
		{
			Print(
				std::format("INFO: Execute(): SetCurrencyAndTimeframe() has succeeded. currency='{}', timeframe={}", orderSymbol, timeframe)
			);
		}
		else
		{
			Print(
				std::format("ERRO: Execute(): SetCurrencyAndTimeframe() has failed. currency='{}', timeframe={}", orderSymbol, timeframe)
			);
			return;
		}

		for(int i = -1; i <= maxRLevels; ++i)
		{
			const double price = openPrice + oneR * i;
			const auto objectName = std::format("RLevel_{}", i);

			if(ObjectExists(objectName))
			{
				Print(
					std::format("INFO: Execute(): Object '{}' exists. Deleting it.", objectName)
				);
				ObjectDelete(objectName);
			}
			else
			{
				Print(
					std::format("INFO: Execute(): Object '{}' does NOT exist.", objectName)
				);
			}

			if(!ObjectCreate(
				objectName,
				obj_HLine,
				0,
				0.0,
				price
				)
			)
			{
				Print(
					std::format("ERRO: Execute(): ObjectCreate() has failed. objectName='{}', price={}", objectName, price)
				);
				return;
			}
			else
			{
				Print(
					std::format("INFO: Execute(): ObjectCraete() has succeeded. objectName='{}', price={}", objectName, price)
				);

				std::string objectDescription = std::format("{}R", i);

				if(ObjectSetText(objectName, objectDescription, 8, "Arial", clRed))
				{
					Print(
						std::format("INFO: Execute(): ObjectSetText() has succeeded. objectName='{}', objectDescription={}", objectName, objectDescription)
					);
				}
				else
				{
					Print(
						std::format("ERRO: Execute(): ObjectSetText() has failed. objectName='{}', objectDescription={}", objectName, objectDescription)
					);
					return;
				}

				{
					const int objectProp = OBJPROP_COLOR;
					const double objectColor = clLightGray;

					if(ObjectSet(objectName, objectProp, objectColor))
					{
						Print(
							std::format("INFO: Execute(): ObjectSet() has succeeded. objectName='{}', objectProp={}, objectColor = {}", objectName, objectProp, objectColor)
						);
					}
					else
					{
						Print(
							std::format("INFO: Execute(): ObjectSet() has failed. objectName='{}', objectProp={}, objectColor = {}", objectName, objectProp, objectColor)
						);
						return;
					}
				}
			}
			{
				const int objectProp = OBJPROP_WIDTH;
				const double objectWidth = 1;

				if(ObjectSet(objectName, objectProp, objectWidth))
				{
					Print(
						std::format("INFO: Execute(): ObjectSet() has succeeded. objectName='{}', objectProp={}, objectWidth = {}", objectName, objectProp, objectWidth)
					);
				}
				else
				{
					Print(
						std::format("INFO: Execute(): ObjectSet() has failed. objectName='{}', objectProp={}, objectWidth = {}", objectName, objectProp, objectWidth)
					);
					return;
				}
			}
		}
	}
	else
	{
		Print(
			std::format("ERRO: Execute(): OrderSelect() has failed.")
		);
		return;
	}
}
