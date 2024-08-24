// This file added in headers queue
// File: "Sources.h"
#include "resource.h"

namespace GOTHIC_ENGINE {
    zCView* mainView;
    zCView* btnView;
    zCView* btnView2;
    zCView* itemView;
    zCView* itemView2;
    zCWorld* itemWorld;
    int SizeX = 4096;
    int SizeY = 4096;

    bool Show = false;

    void Test_Render_Init() {
        mainView = new zCView(1024, 1024, 4096, 4096);
        mainView->InsertBack("AlchemyScreen_Background.tga");

        btnView = new zCView(500, 1024, SizeX, SizeY);
        btnView2 = new zCView(500, 3024, SizeX, SizeY + 2000);
        itemView = new zCView(0, 0, SizeX, SizeY);
        itemView2 = new zCView(0, 500, SizeX, SizeY);
        itemWorld = zfactory->CreateWorld();
#if ENGINE == Engine_G2A
        itemWorld->m_bIsInventoryWorld = TRUE;
#endif
    }

    void Test_Render_Exit() {
        if (mainView) {
            delete mainView;
            mainView = nullptr;
        }

        if (itemView) {
            delete itemView;
            itemView = nullptr;
        }

        if (itemView2) {
            delete itemView2;
            itemView2 = nullptr;
        }
    }

    void Test_Render_Loop(int keyToShow) {
        if (!ogame || ogame->IsOnPause()) {
            return;
        }

        if (zinput->KeyToggled(keyToShow)) {
            Show = !Show;
        }

        if (!Show) {
            return;
        }

        if (!mainView || !itemView) {
            return;
        }

        //auto item = player->IsInInv(zSTRING{ "ItPo_Mana_01" }, 1);
        //auto item2 = player->IsInInv(zSTRING{ "ItPo_Health_02" }, 1);

        oCItem* item = new oCItem((zSTRING)"ItPo_Mana_01", 1);
        oCItem* item2 = new oCItem((zSTRING)"ItPo_Health_02", 1);

        if (!item || !item2)
        {
            return;
        }

        /*mainView->InsertBack("AlchemyScreen_Background.tga");
        btnView->InsertBack("AlchemyScreen_Button_Background_2.tga");
        btnView2->InsertBack("AlchemyScreen_Button_Background_2.tga");*/

        screen->InsertItem(mainView);

        mainView->Print(1024, 1024, "HiHi");
        mainView->Blit();

        auto prevLighting = oCItem::GetLightingSwell();


        oCItem::SetLightingSwell(TRUE);
        mainView->InsertItem(itemView);
        itemView->SetPos(4096 - (SizeX / 2), 4096 - (SizeY / 2));
        itemView->SetSize(SizeX, SizeY);
        item2->RenderItem(itemWorld, itemView, 0.0f);
        mainView->RemoveItem(itemView);

        oCItem::SetLightingSwell(TRUE);
        mainView->InsertItem(itemView2);
        itemView2->SetPos(4096 - (SizeX / 2) + 500, 4096 - (SizeY / 2) + 500);
        itemView2->SetSize(SizeX, SizeY);
        item->RenderItem(itemWorld, itemView2, 0.0f);
        mainView->RemoveItem(itemView2);

        oCItem::SetLightingSwell(prevLighting);
        screen->RemoveItem(mainView);

        item->Release();
        item2->Release();

    }

    void KeyPressedFunction() {
        if (zinput->KeyToggled(KEY_L)) /* keyCode: e.g. KEY_L, KEY_LALT, etc. */ {
            screen->Print(200, 400, "Przycisk naciœniêty: L");
            NewStatsMenu_Hide();
            //ShowCursor(1);   //Shows system cursor
            CraftingView::Close();
        }
        else if (zinput->KeyToggled(KEY_K)) {
            screen->Print(200, 400, "Przycisk naciœniêty: K");
            //NewStatsMenu_ChangeColor();
            //NewStatsMenu_Show();
            //ShowCursor(0);  //Hide system cursor (but it works in a weird way)
            
            //screen->RemoveItem(mainView);
            //mainView->RemoveItem(btnView2);
            mainView->Close();
        }
        else if (zinput->KeyToggled(KEY_J)) {
            screen->Print(200, 400, "Przycisk naciœniêty: J");
            //EndInteractionWithMob(player);

            CraftingView::Open_Alchemy();
        }
        else if (zinput->KeyToggled(KEY_U)) {
            screen->Print(200, 400, "Przycisk naciœniêty: U");
            
            /*int itemInd = parser->GetIndex("ItPo_Mana_01");
            int itemInd2 = parser->GetIndex("ItPo_Mana_02");
            int funcInd = parser->GetIndex("Test_RenderPotion");
            int funcInd2 = parser->GetIndex("Test_RenderPotion2");
            parser->CallFunc(funcInd2, itemInd, itemInd2);
            parser->CallFunc(funcInd, itemInd, itemInd2);*/

        }
        else if (zinput->KeyToggled(KEY_5)) {
            CraftingView::view->InsertBack("AlchemyScreen_Background_1.tga");
        }
        else if (zinput->KeyToggled(KEY_6)) {
            CraftingView::view->InsertBack("AlchemyScreen_Background_2.tga");
        }
        else if (zinput->KeyToggled(KEY_7)) {
            CraftingView::view->InsertBack("AlchemyScreen_Background_3.tga");
        }
        else if (zinput->KeyToggled(KEY_8)) {
            CraftingView::view->InsertBack("AlchemyScreen_Background_4.tga");
        }
        
    }


  void Game_Entry() {
  }
  
  void Game_Init() {
      CursorStatic::Init();
      CraftingView::Init();
      menu_View = new zCView(0, 0, 8192, 8192);
      

      //Test_Render_Init();
  }

  void Game_Exit() {
      //Test_Render_Exit();
  }

  void Game_PreLoop() {
  }

  void Game_Loop() {
      KeyPressedFunction();
      CraftingView::Update();
      CursorStatic::Update();


      //Test_Render_Loop(KEY_Y);
  }

  void Game_PostLoop() {
  }

  void Game_MenuLoop() {
  }

  // Information about current saving or loading world
  TSaveLoadGameInfo& SaveLoadGameInfo = UnionCore::SaveLoadGameInfo;

  void Game_SaveBegin() {
  }

  void Game_SaveEnd() {
  }

  void LoadBegin() {
  }

  void LoadEnd() {
  }

  void Game_LoadBegin_NewGame() {
    LoadBegin();
  }

  void Game_LoadEnd_NewGame() {
    LoadEnd();
  }

  void Game_LoadBegin_SaveGame() {
    LoadBegin();
  }

  void Game_LoadEnd_SaveGame() {
    LoadEnd();
  }

  void Game_LoadBegin_ChangeLevel() {
    LoadBegin();
  }

  void Game_LoadEnd_ChangeLevel() {
    LoadEnd();
  }

  void Game_LoadBegin_Trigger() {
  }
  
  void Game_LoadEnd_Trigger() {
  }
  
  void Game_Pause() {
  }
  
  void Game_Unpause() {
  }
  
  void Game_DefineExternals() {
      //parser->DefineExternal("TestExternal", NewStatsMenu_Show_External, zPAR_TYPE_INT, zPAR_TYPE_VOID);
      //parser->DefineExternal("NewStatsMenu_Hide_External", NewStatsMenu_Hide_External, zPAR_TYPE_INT, zPAR_TYPE_VOID);
  }

  void Game_ApplyOptions() {
  }

  /*
  Functions call order on Game initialization:
    - Game_Entry           * Gothic entry point
    - Game_DefineExternals * Define external script functions
    - Game_Init            * After DAT files init
  
  Functions call order on Change level:
    - Game_LoadBegin_Trigger     * Entry in trigger
    - Game_LoadEnd_Trigger       *
    - Game_Loop                  * Frame call window
    - Game_LoadBegin_ChangeLevel * Load begin
    - Game_SaveBegin             * Save previous level information
    - Game_SaveEnd               *
    - Game_LoadEnd_ChangeLevel   *
  
  Functions call order on Save game:
    - Game_Pause     * Open menu
    - Game_Unpause   * Click on save
    - Game_Loop      * Frame call window
    - Game_SaveBegin * Save begin
    - Game_SaveEnd   *
  
  Functions call order on Load game:
    - Game_Pause              * Open menu
    - Game_Unpause            * Click on load
    - Game_LoadBegin_SaveGame * Load begin
    - Game_LoadEnd_SaveGame   *
  */

#define AppDefault True
  CApplication* lpApplication = !CHECK_THIS_ENGINE ? Null : CApplication::CreateRefApplication(
    Enabled( AppDefault ) Game_Entry,
    Enabled( AppDefault ) Game_Init,
    Enabled( AppDefault ) Game_Exit,
    Enabled( AppDefault ) Game_PreLoop,
    Enabled( AppDefault ) Game_Loop,
    Enabled( AppDefault ) Game_PostLoop,
    Enabled( AppDefault ) Game_MenuLoop,
    Enabled( AppDefault ) Game_SaveBegin,
    Enabled( AppDefault ) Game_SaveEnd,
    Enabled( AppDefault ) Game_LoadBegin_NewGame,
    Enabled( AppDefault ) Game_LoadEnd_NewGame,
    Enabled( AppDefault ) Game_LoadBegin_SaveGame,
    Enabled( AppDefault ) Game_LoadEnd_SaveGame,
    Enabled( AppDefault ) Game_LoadBegin_ChangeLevel,
    Enabled( AppDefault ) Game_LoadEnd_ChangeLevel,
    Enabled( AppDefault ) Game_LoadBegin_Trigger,
    Enabled( AppDefault ) Game_LoadEnd_Trigger,
    Enabled( AppDefault ) Game_Pause,
    Enabled( AppDefault ) Game_Unpause,
    Enabled( AppDefault ) Game_DefineExternals,
    Enabled( AppDefault ) Game_ApplyOptions
  );
}