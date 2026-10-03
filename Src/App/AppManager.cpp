#include "AppManager.hpp"
#include "sdkconfig.h"
#if APP_USE_COMM
#include "CommManager.hpp"
#endif

#include "Cmd.hpp"
#include "Bsp.hpp"
#include "StmLog.hpp"
#include "App.hpp"
#include <memory>

#if EXTENSION_BUZZER
#include "Buzzer.hpp"
#endif

#if EXTENSION_LED
#include "LEDManager.hpp"
#endif

#if APP_USE_DAEMONS
#include "Daemons.hpp"
#endif

//---------------------------------------------------------------------------------------------------

static std::unique_ptr<Cmd> cmd;

//---------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------

#if APP_USE_UI
#include "UI/UIApp.hpp"
#endif

//---------------------------------------------------------------------------------------------------

void AppManager::initApp()
{
#if EXTENSION_BUZZER
    BUZZER::buzz.init(&BEEP_TIMER, BEEP_TIM_CHANNEL, BEEP_APB_FREQ);
    BUZZER::buzz->playNote(BUZZER::PinyCore);
#endif

    cmd = std::make_unique<Cmd>();

#if APP_USE_DAEMONS
    Daemons::instance().init();
    schedule([]() { Daemons::instance().update(); });
#endif

    App::instance().init();
    schedule([]() { App::instance().task(); });
    this->createApp();
}

void AppManager::createApp()
{
    uint32_t freeHeap = xPortGetFreeHeapSize();
    LOG::info("App", "init complete, Free Heap: %u", freeHeap);
}

void AppManager::task()
{
    while (true) {
        for (auto &task : this->tasks) {
            task();
        }
        vTaskDelay(1);
    }
}