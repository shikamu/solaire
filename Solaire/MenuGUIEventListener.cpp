#include <cassert>
#include <irrlicht.h>

#include "MenuGUIEventListener.h"
#include "GUIConstants.h"
#include "System.h"
#include "ConfigData.h"
#include "MenuScene.h"
#include "NetworkController.h"
#include "LANFinalView.h"
#include "LANServer.h"
#include "AudioManager.h"

using namespace irr;


MenuGUIEventListener::MenuGUIEventListener(MenuScene* scene) : m_scene(scene)
{
}

MenuGUIEventListener::~MenuGUIEventListener()
{
}

bool MenuGUIEventListener::onGUIEvent(const irr::SEvent::SGUIEvent& evt)
{
	static const video::SColor default_EGDC_3D_HIGH_LIGHT = System::get().getDevice()->getGUIEnvironment()->getSkin()->getColor(gui::EGDC_3D_HIGH_LIGHT);
	static const video::SColor default_EGDC_SCROLLBAR = System::get().getDevice()->getGUIEnvironment()->getSkin()->getColor(gui::EGDC_SCROLLBAR);

	static s32 lastRow = 0;

	s32 id = evt.Caller->getID();
	
	switch(evt.EventType)
	{
		case gui::EGET_BUTTON_CLICKED:
			switch(id)
			{
				case GUI_ID_MAINMENU_SINGLE_BUTTON:
				{
					FMODAudio::AudioManager::get().SelectPlaylist("Game");
					m_scene->showSinglePlayerView();
					return true;
				}
				case GUI_ID_MAINMENU_LAN_BUTTON:
					FMODAudio::AudioManager::get().SelectPlaylist("Menu");
					m_scene->showLANView();
					return true;
				case GUI_ID_MAINMENU_OPTIONS_BUTTON:
					m_scene->showOptionsView();
					return true;
				case GUI_ID_MAINMENU_EXIT_BUTTON:
					System::get().getDevice()->closeDevice();
					return true;
				case GUI_ID_SINGLEMENU_PLAY_BUTTON:
					//System::get().toggleSinglePlayerScene();
					System::get().toggle(SCENE_SINGLE);
					return true;
				case GUI_ID_SINGLEMENU_PREVIOUS_BUTTON:
				{
					FMODAudio::AudioManager::get().PrevTrack();
					m_scene->togglePreviousShip();
					return true;
				}
				case GUI_ID_SINGLEMENU_NEXT_BUTTON:
				{
					FMODAudio::AudioManager::get().NextTrack();
					m_scene->toggleNextShip();
					return true;
				}
				case GUI_ID_OPTIONSMENU_APPLY_BUTTON:
				{
					System::get().setRunning(true);
					System::get().getDevice()->closeDevice();
					return true;
				}
				case GUI_ID_LANMENU_CREATE_BUTTON:
				{
					evt.Caller->setEnabled(false);
					gui::IGUIEnvironment* env = System::get().getDevice()->getGUIEnvironment();
					gui::IGUIWindow* msgbox = env->addMessageBox(L"Enter a game name", NULL, true, (gui::EMBF_OK | gui::EMBF_CANCEL), NULL, GUI_ID_LANMENU_CREATENEWGAME_MESSAGEBOX);
					msgbox->setMinSize(core::dimension2du(380, 140));
					core::list<gui::IGUIElement*> children = msgbox->getChildren();
					for(core::list<gui::IGUIElement*>::ConstIterator it = children.begin(); it != children.end(); ++it)
					{
						gui::IGUIElement* elt = (*it);
						if(elt->hasType(gui::EGUIET_BUTTON))
						{
							gui::IGUIButton* but = (gui::IGUIButton*)elt;
							if(!_wcsicmp(but->getText(), L"OK"))
							{
								s32 h = but->getAbsolutePosition().getHeight();
								but->setRelativePosition(core::position2di(but->getRelativePosition().UpperLeftCorner.X, msgbox->getAbsolutePosition().getHeight()-h-5));
							}
							else if(!_wcsicmp(but->getText(), L"CANCEL"))
							{
								s32 h = but->getAbsolutePosition().getHeight();
								but->setRelativePosition(core::position2di(but->getRelativePosition().UpperLeftCorner.X, msgbox->getAbsolutePosition().getHeight()-h-5));
							}
						}
					}

					gui::IGUIStaticText* text = env->addStaticText(L"Game Name: ", core::rect<s32>(10, 25, 125, 55), false, true, msgbox);
					gui::IGUIEditBox* name = env->addEditBox(L"", core::rect<s32>(130, 25, 360, 55), true, msgbox, GUI_ID_LAN_CREATENEWGAME_GAMENAME_EDITBOX);
					name->setMax(20);
					env->setFocus(name);
					text = env->addStaticText(L"Your Name: ", core::rect<s32>(10, 60, 125, 90), false, true, msgbox);
					name = env->addEditBox(L"", core::rect<s32>(130, 60, 360, 90), true, msgbox, GUI_ID_LAN_CREATENEWGAME_HOSTNAME_EDITBOX);
					name->setMax(14);
					return true;
				}
				case GUI_ID_LANMENU_JOIN_BUTTON:
				{
					//NetworkController::get().unregisterLANServer();
					gui::IGUIEnvironment* env = System::get().getDevice()->getGUIEnvironment();
					gui::IGUITable* lobby = (gui::IGUITable*)env->getRootGUIElement()->getElementFromId(GUI_ID_LANMENU_TABLE, true);
					if(lobby)
					{
						s32 sel = lobby->getSelected();
						if(sel < 0 && lobby->getRowCount() > 0)
						{
							sel = 0;
							lobby->setSelected(sel);
						}
						if(sel >= 0)
						{
							gui::IGUIWindow* msgbox = env->addMessageBox(L"Choose a nickname", NULL, true, (gui::EMBF_OK | gui::EMBF_CANCEL), NULL, GUI_ID_LANMENU_JOINGAME_MESSAGEBOX);
							msgbox->setMinSize(core::dimension2du(380, 90));
							gui::IGUIStaticText* text = env->addStaticText(L"Nickname: ", core::rect<s32>(10, 25, 125, 55), false, true, msgbox);
							gui::IGUIEditBox* name = env->addEditBox(L"", core::rect<s32>(130, 25, 360, 55), true, msgbox, GUI_ID_LAN_JOINGAME_NICKNAME_EDITBOX);
							name->setMax(14);
							env->setFocus(name);
						}
					}
					return true;
				}
				case GUI_ID_LANFINAL_STARTGAME_BUTTON:
				{
					if(evt.Caller->hasType(gui::EGUIET_BUTTON))
					{
						gui::IGUIButton* startgame = ((gui::IGUIButton*)evt.Caller);
						LANServer* server = NetworkController::get().getServer();
						if(server)
						{
							LANFinalView* view = NetworkController::get().getLANFinalView();
							if(view)
							{
								server->setDisconnectAll(false);
								startgame->setEnabled(false);
								view->switchToLANScene();
								return true;
							}
						}
					}
					break;
				}
				case GUI_ID_LAN_TEST_BUTTON:
				{
					//NetworkController::get().unregisterLANServer();
					int connect = m_scene->connectToGame(L"127.0.0.1", L"Test");
					if(connect != 0)
					{
						core::stringc errorMsg("omfg, connect didn't succeed! returned code=");
						errorMsg+=connect;
						System::get().log(errorMsg.c_str());
						generateErrorMessageBoxForJoinFailure(connect);
					}
					else
					{
						System::get().log("blimey, the server accepted us!");
						m_scene->showLANClientView(L"Test");
					}

					return true;
				}
				default:
					return false;
			}
			break;
		case gui::EGET_MESSAGEBOX_OK:
			switch(id)
			{
				case GUI_ID_LANMENU_CREATENEWGAME_MESSAGEBOX:
				{
					createLANNewGame();
					break;
				}
				case GUI_ID_LANMENU_JOINGAME_MESSAGEBOX:
				{
					joinLANGame();
					break;
				}
			}
			break;
		case gui::EGET_MESSAGEBOX_CANCEL:
			switch(id)
			{
				case GUI_ID_LANMENU_CREATENEWGAME_MESSAGEBOX:
				{
					gui::IGUIElement* createButton = System::get().getDevice()->getGUIEnvironment()->getRootGUIElement()->getElementFromId(GUI_ID_LANMENU_CREATE_BUTTON, true);
					if(createButton)
						createButton->setEnabled(true);
					break;
				}
			}
			break;
		case gui::EGET_COMBO_BOX_CHANGED:
			switch(id)
			{
				case GUI_ID_OPTIONSMENU_VIDEODRIVER_COMBO:
				{
					gui::IGUIComboBox* videoDriver = ((gui::IGUIComboBox*)evt.Caller);
					System::get().getConfig()->setDriverType((video::E_DRIVER_TYPE) videoDriver->getItemData(videoDriver->getSelected()));
					return true;
				}
				case GUI_ID_OPTIONSMENU_VIDEOMODE_COMBO:
				{
					gui::IGUIComboBox* videoMode = ((gui::IGUIComboBox*)evt.Caller);
					u32 val = videoMode->getItemData(videoMode->getSelected());
					System::get().getConfig()->setWindowWidth(val >> 16);
					System::get().getConfig()->setWindowHeight(val & 0xFFFF);
					return true;
				}
				default:
					return false;
			}
			break;
		case gui::EGET_EDITBOX_ENTER:
			switch(id)
			{
				case GUI_ID_LANFINAL_INPUTCHAT_EDITBOX:
				{
					/*
					gui::IGUIEditBox* chat = (gui::IGUIEditBox*)System::get().getDevice()->getGUIEnvironment()->getRootGUIElement()->getElementFromId(GUI_ID_LANFINAL_CHAT_EDITBOX, true);
					assert(chat);
					*/
					gui::IGUIEditBox* input = (gui::IGUIEditBox*)evt.Caller;
			
					LANFinalView* view = NetworkController::get().getLANFinalView();
					if(view)
					{
						view->sendChatText(m_scene->getPlayerName(), input->getText());
					}

					input->setText(L"");
					/*
					core::stringw text(chat->getText());
					text+=L"\n";
					text+=m_scene->getPlayerName();
					text+=L": ";
					text+=input->getText();
					input->setText(L"");
					chat->setText(text.c_str());
					*/
					break;
				}
				case GUI_ID_LAN_CREATENEWGAME_HOSTNAME_EDITBOX:
				{
					createLANNewGame();
					
					gui::IGUIElement* root = System::get().getDevice()->getGUIEnvironment()->getRootGUIElement();
					gui::IGUIWindow* view = (gui::IGUIWindow*)root->getElementFromId(GUI_ID_LANMENU_CREATENEWGAME_MESSAGEBOX, true);
					
					if(view)
					{
						view->remove();
					}
					
					break;
				}
				case GUI_ID_LAN_JOINGAME_NICKNAME_EDITBOX:
				{
					joinLANGame();

					gui::IGUIElement* root = System::get().getDevice()->getGUIEnvironment()->getRootGUIElement();
					gui::IGUIElement* view = root->getElementFromId(GUI_ID_LANMENU_JOINGAME_MESSAGEBOX, true);
					if(view)
					{
						view->remove();
					}
					break;
				}
			}
			break;
		case gui::EGET_CHECKBOX_CHANGED:
			switch(id)
			{
				case GUI_ID_OPTIONSMENU_FULLSCREEN_CHECKBOX:
				{
					gui::IGUICheckBox* fullscreen = ((gui::IGUICheckBox*)evt.Caller);
					System::get().getConfig()->setFullscreen(fullscreen->isChecked());
					return true;
				}
				case GUI_ID_OPTIONSMENU_32BITS_CHECKBOX:
				{
					gui::IGUICheckBox* bits = ((gui::IGUICheckBox*)evt.Caller);
					System::get().getConfig()->setBits(bits->isChecked() ? 32 : 16);
					return true;
				}
				case GUI_ID_OPTIONSMENU_VSYNC_CHECKBOX:
				{
					gui::IGUICheckBox* vsync = ((gui::IGUICheckBox*)evt.Caller);
					System::get().getConfig()->setVSync(vsync->isChecked());
					return true;
				}

				default:
					return false;
			}
			break;
		case gui::EGET_SCROLL_BAR_CHANGED:
			switch(id)
			{
				case GUI_ID_OPTIONSMENU_MULTISAMPLE_BAR:
				{
					gui::IGUIScrollBar* multisample = ((gui::IGUIScrollBar*)evt.Caller);
					System::get().getConfig()->setAntiAlias(multisample->getPos());
					return true;
				}
				case GUI_ID_OPTIONSMENU_GAMMA_BAR:
				{
					gui::IGUIScrollBar* gamma = ((gui::IGUIScrollBar*)evt.Caller);					
					System::get().getConfig()->setGamma(gamma->getPos() * 0.01f);
					return true;
				}
				default:
					return false;
			}
			break;
		case gui::EGET_TAB_CHANGED:
			switch(id)
			{
				case GUI_ID_OPTIONSMENU_VIDEODRIVER_TAB:
				{
					gui::IGUITabControl* tab = ((gui::IGUITabControl*)evt.Caller);
					gui::IGUIEnvironment* env = System::get().getDevice()->getGUIEnvironment();

					env->getSkin()->setColor(gui::EGDC_3D_HIGH_LIGHT, default_EGDC_3D_HIGH_LIGHT);
					env->getSkin()->setColor(gui::EGDC_SCROLLBAR, default_EGDC_SCROLLBAR);
					//if the controls tab is active
					if(tab->getActiveTab() == 2)
					{
						m_scene->onShowControlsPanel();
					}
					else
					{
						m_scene->onHideControlsPanel();
						m_scene->unlockKeybindingTable();
					}
					return true;
				}
				default:
					return false;
			}
			break;
		case gui::EGET_TABLE_CHANGED:
		case gui::EGET_TABLE_SELECTED_AGAIN:
			switch(id)
			{
				case GUI_ID_OPTIONSMENU_KEYBINDINGS_TABLE:
				{
					gui::IGUITable* table = ((gui::IGUITable*)evt.Caller);
					if(m_scene->isKeybindingTableLocked())
					{
						table->setSelected(lastRow);
					}
					else
					{
						lastRow = table->getSelected();
					}

					return true;
				}
				default:
					return false;
			}
			break;
		default:
			break;
	}
	return false;
}

void MenuGUIEventListener::generateErrorMessageBoxForJoinFailure(const int errorCode)
{

	core::stringw errorMessage;
	switch(errorCode)
	{
		default:
			errorMessage = L"Failed to join the game for unknown reason.";
			break;
		case 1:
			errorMessage = L"Problem with getaddrinfo(), ip address or port may be corrupted or invalid.";
			break;
		case 2:
			errorMessage = L"Failed to create socket.";
			break;
		case 3:
			errorMessage = L"Failed to connect to the server.";
			break;
		case 4:
			errorMessage = L"Failed to send our name to the server.";
			break;
		case 5:
			errorMessage = L"Couldn't create packet for sending our name to the server.";
			break;
		case 6:
			errorMessage = L"You're trying to connect from a view that is not the LAN view. Hax?";
			break;
		case 7:
			errorMessage = L"Server did not accept our name.";
			break;
		case 8:
			errorMessage = L"Failed to configure our socket (ioctlsocket failure).";
			break;
	}

	gui::IGUIWindow* msgbox = System::get().getDevice()->getGUIEnvironment()->addMessageBox(L"Failed to join the selected game", errorMessage.c_str(), true, gui::EMBF_OK);
	msgbox->setMinSize(core::dimension2du(300, 100));
	core::list<gui::IGUIElement*> children = msgbox->getChildren();
	for(core::list<gui::IGUIElement*>::ConstIterator it = children.begin(); it != children.end(); ++it)
	{
		gui::IGUIElement* elt = (*it);
		if(elt->hasType(gui::EGUIET_BUTTON))
		{
			gui::IGUIButton* but = (gui::IGUIButton*)elt;
			if(!_wcsicmp(but->getText(), L"OK"))
			{
				s32 h = but->getAbsolutePosition().getHeight();
				s32 w = but->getAbsolutePosition().getWidth();
				but->setRelativePosition(core::position2di(static_cast<s32>((msgbox->getAbsolutePosition().getWidth() - w)*0.5f), msgbox->getAbsolutePosition().getHeight()-h-5));
			}
		}
	}
}

void MenuGUIEventListener::createLANNewGame()
{
	gui::IGUIElement* root = System::get().getDevice()->getGUIEnvironment()->getRootGUIElement();
	gui::IGUIEditBox* gamename = (gui::IGUIEditBox*)root->getElementFromId(GUI_ID_LAN_CREATENEWGAME_GAMENAME_EDITBOX, true);
	gui::IGUIEditBox* hostname = (gui::IGUIEditBox*)root->getElementFromId(GUI_ID_LAN_CREATENEWGAME_HOSTNAME_EDITBOX, true);
					
	assert(root);
	assert(gamename);
	assert(hostname);

	m_scene->showLANServerView(hostname->getText(), gamename->getText());
	gui::IGUIElement* createButton = root->getElementFromId(GUI_ID_LANMENU_CREATE_BUTTON, true);
	if(createButton)
		createButton->setEnabled(true);//make sure this is done last

}

void MenuGUIEventListener::joinLANGame()
{
	gui::IGUIEnvironment* env = System::get().getDevice()->getGUIEnvironment();
	gui::IGUITable* lobby = (gui::IGUITable*)env->getRootGUIElement()->getElementFromId(GUI_ID_LANMENU_TABLE, true);
	if(lobby)
	{
		s32 sel = lobby->getSelected();
		/*
		if(sel < 0 && lobby->getRowCount() > 0)
			sel = 0;
		*/
		if(sel >= 0)
		{
			gui::IGUIEditBox* chat = (gui::IGUIEditBox*)System::get().getDevice()->getGUIEnvironment()->getRootGUIElement()->getElementFromId(GUI_ID_LAN_JOINGAME_NICKNAME_EDITBOX, true);
			core::stringw ipAddress(lobby->getCellText(sel, 1));
			core::stringw nickname(chat->getText());
			int connect = m_scene->connectToGame(ipAddress.c_str(), nickname.c_str());
			if(connect != 0)
			{
				core::stringc errorMsg("omfg, connect didn't succeed! returned code=");
				errorMsg+=connect;
				System::get().log(errorMsg.c_str());
				generateErrorMessageBoxForJoinFailure(connect);
			}
			else
			{
				System::get().log("blimey, the server accepted us!");
				m_scene->showLANClientView(nickname.c_str());
			}
		}
	}
}
