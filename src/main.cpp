#include "include.h"
#include <Geode/modify/LoadingLayer.hpp>
#include "Keybinds/SetBindSetting.hpp"
#include "Keybinds/RecordKeyPopup.hpp"

#ifdef GEODE_IS_WINDOWS
//#define IMGUI
#endif

bool showing = false;

#ifdef IMGUI
bool android = false;
#else
bool android = true;
#endif

Client* client;

class $modify (CCKeyboardDispatcher)
{
    bool dispatchKeyboardMSG(enumKeyCodes key, bool down, bool idk)
    {
        if (!CCScene::get())
            return CCKeyboardDispatcher::dispatchKeyboardMSG(key, down, idk);

        if (!getChildOfType<LoadingLayer>(CCScene::get(), 0) && !getChildOfType<RecordKeyPopup>(CCScene::get(), 0))
        { 
            bool v = false;

            std::vector<int> btns = { enumKeyCodes::KEY_Tab, enumKeyCodes::KEY_F12, enumKeyCodes::KEY_Insert };

            if (SetBindValue::instance)
                btns = SetBindValue::instance->buttons;

            for (auto btn : btns)
            {
                if (btn == key)
                    v = true;
            }

            if (down && v && !idk) {
                if (android)
                {
                    #ifdef GEODE_IS_WINDOWS
                    CCDirector::get()->getOpenGLView()->showCursor(true);
                    #endif

                    if (auto ui = getChildOfType<AndroidUI>(CCScene::get(), 0))
                    {
                        ui->onClose(nullptr);
                    }
                    else
                    {
                        AndroidUI::addToScene();
                    }
                }
                else
                {
                    showing = !showing;
                    Client::instance->open = showing;
                }
            }
        }

        if (!android)
        {
            if (key == KEY_Escape)
            {
                if (InputModule::selected)
                {
                    InputModule::selected = nullptr;
                    return true;
                }
            }

            if (InputModule::selected)
            {
                if (down)
                {    
                    if (key >= 48)
                    {
                        if (key <= 90)
                        {
                            std::stringstream ss;

                            if (CCKeyboardDispatcher::getShiftKeyPressed())
                                ss << CCKeyboardDispatcher::keyToString(key);
                            else
                            {
                                ss << CCKeyboardDispatcher::keyToString(key);
                            }

                            if (InputModule::selected->text.length() < InputModule::selected->maxSize)
                            {
                                for (size_t i = 0; i < InputModule::selected->allowedChars.length(); i++)
                                {
                                    if (InputModule::selected->allowedChars[i] == ss.str()[0])
                                    {
                                        InputModule::selected->text += ss.str();
                                    }
                                }
                            }

                            return true;
                        }
                    }

                    if (key == KEY_Backspace)
                    {
                        InputModule::selected->text = InputModule::selected->text.substr(0, InputModule::selected->text.length() - 1);
                    }

                    if (key == 190)
                    {
                        if (InputModule::selected->text.length() < InputModule::selected->maxSize)
                            {
                                for (size_t i = 0; i < InputModule::selected->allowedChars.length(); i++)
                                {
                                    if (InputModule::selected->allowedChars[i] == "."[0])
                                    {
                                        InputModule::selected->text += ".";
                                    }
                                }
                            }
                    }

                    return true;
                }
            }
        }

        return CCKeyboardDispatcher::dispatchKeyboardMSG(key, down, idk);
    }
};

void migrateData()
{
    if (Mod::get()->getSavedValue<bool>("migrated"))
        return;
    
    auto path = Mod::get()->getSaveDir().string();
    path = utils::string::replace(path, Mod::get()->getID(), "TheSillyDoggo.Cheats");

    auto saved = path + "\\saved.json";
    auto savedNew = Mod::get()->getSaveDir().string() + "\\saved.json";

    log::debug("Old Config Path: {}", saved);
    log::debug("Old Config Path Exists: {}", std::filesystem::exists(saved));

    log::debug("Old Config Path: {}", savedNew);
    log::debug("Old Config Path Exists: {}", std::filesystem::exists(savedNew));

    if (std::filesystem::exists(saved))
    {
        auto res = std::filesystem::copy_file(saved, savedNew, std::filesystem::copy_options::skip_existing);

        log::debug("Copy saved results: {}", res);
    }

    auto res = Mod::get()->loadData();

    if (res.has_error())
        log::error("Error loading: {}", res.error());

    Mod::get()->setSavedValue<bool>("migrated", true);
}

$execute
{
    migrateData();

    //android = !Mod::get()->getSettingValue<bool>("use-new-ui");
    /*
    ,
    "use-new-ui": {
        "type": "bool",
        "name": "[BETA] Use PC Gui",
        "default": false,
        "description": "Uses a UI more comfortable for pc users, like Mega Hack. Requires Game Restart",
        "platforms": [ "win", "macos" ]
    }
    */

    client = new Client();
    Client::instance = client;

    ClientUtils::Setup(android);
}

#ifdef IMGUI

void DrawDescription()
{
    ImVec2 pos = ImVec2(10, ImGui::GetIO().DisplaySize.y - 10);

    //if (!AltModuleLocation::instance->enabled)
    if (true)
    {
        pos = ImGui::GetMousePos();

        if (pos.x + ImGui::CalcTextSize(Module::descMod.c_str()).x + 20 > ImGui::GetIO().DisplaySize.x)
        {
            pos.x -= ImGui::CalcTextSize(Module::descMod.c_str()).x + 20;
        }
    }

    DrawUtils::drawRect(ImVec2(pos.x, pos.y - (ImGui::CalcTextSize(Module::descMod.c_str()).y + 20)), ImVec2(ImGui::CalcTextSize(Module::descMod.c_str()).x + 20, ImGui::CalcTextSize(Module::descMod.c_str()).y + 20), ColourUtility::GetColour(ColourUtility::ClientColour::InputField));
    DrawUtils::anchoredText(ImVec2(pos.x + 10, pos.y - 10 - (ImGui::CalcTextSize(Module::descMod.c_str()).y)), ImVec2(ImGui::CalcTextSize(Module::descMod.c_str()).x, ImGui::CalcTextSize(Module::descMod.c_str()).y), Module::descMod.c_str(), ImColor(255, 255, 255, 255), ImVec2(0, 0));
}

#endif

bool v = false;

class $modify (MenuLayer)
{
    virtual bool init()
    {
        if (!MenuLayer::init())
            return false;

        if (!v)
        {
            if (Client::GetModuleEnabled("save-pos"))
            {
                AndroidBall::position = ccp(Mod::get()->getSavedValue("posX", 32), Mod::get()->getSavedValue("posY", CCDirector::get()->getWinSize().height / 2));

                if (AndroidBall::position.x < 0)
                    AndroidBall::position.x = 0;

                if (AndroidBall::position.y < 0)
                    AndroidBall::position.y = 0;

                if (AndroidBall::position.x > CCDirector::get()->getWinSize().width)
                    AndroidBall::position.x = CCDirector::get()->getWinSize().width;

                if (AndroidBall::position.y > CCDirector::get()->getWinSize().height)
                    AndroidBall::position.y = CCDirector::get()->getWinSize().height;
            }
            else
            {
                AndroidBall::position = ccp(32, CCDirector::get()->getWinSize().height / 2);
            }
            
            #ifdef IMGUI

            ImGuiCocos::get().setup([] {
                // this runs after imgui has been setup,
                // its a callback as imgui will be re initialized when toggling fullscreen,
                // so use this to setup any themes and or fonts!

                //auto* font = ImGui::GetIO().Fonts->AddFontFromFileTTF((Mod::get()->getResourcesDir() / "verdana.ttf").string().c_str(), 32.0f);

                float mult = 1.0f;

                DrawUtils::mod = ImGui::GetIO().Fonts->AddFontFromFileTTF((Mod::get()->getResourcesDir() / "OpenSans-Regular.ttf").string().c_str(), 18.0f * mult);
                DrawUtils::title = ImGui::GetIO().Fonts->AddFontFromFileTTF((Mod::get()->getResourcesDir() / "OpenSans-Regular.ttf").string().c_str(), 24.0f * mult);
                ImGui::GetIO().FontDefault = DrawUtils::mod;
                ImGui::GetIO().FontGlobalScale = 1 / mult;

            }).draw([] {

                if (!android)
                {
                    if (client->animStatus == 0)
                    {
                        InputModule::selected = nullptr;
                    }

                    if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
                        InputModule::selected = nullptr;

                    client->animStatus += ((showing ? 1 : -1) * ImGui::GetIO().DeltaTime) / 0.25f;
                    client->animStatus = clampf(client->animStatus, 0, 1);

                    client->draw();

                    if (!showing)
                        ImGui::GetIO().WantCaptureMouse = false;

                    if (showing)
                    {
                        if (Module::descMod != "")
                            DrawDescription();
                    }

                    ImGui::End();
                }
            });

            #endif

            v = true;
        }

        return true;
    }
};