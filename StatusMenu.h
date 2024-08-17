// Supported with union (c) 2020 Union team
// Union HEADER file

namespace GOTHIC_ENGINE {
    //Using this throws an error after closing the game (!) - 'Access violation reading location' - but only with zSTRING (if I change it to int, it's fine)
    /*zSTRING menu_Texts_Base[3] = {
        "Postaæ",
        "Poziom",
        "Kr¹g"
    };*/

    

    Menu_Button* button1 = nullptr;

    bool isMenuOpen = false;

    zCView* menu_View = nullptr;         //(PosX, PosY, SizeX, SizeY); 8192 seems to somehow be the Gothic screen size both x and y
    zCView* messages_View = new zCView(0, 0, 8192, 8192);


    



    void NewStatsMenu_ShowPlayerStats() {
        //This code needs to be inside a function that will be called when the game is already running - otherwise the symbol is nullptr (error)
        zCPar_Symbol* sym = parser->GetSymbol("Menu_LP");
        zSTRING symAsString = sym->stringdata;

        //zCPar_Symbol* symbol = parser->GetSymbol("NAME_SPL_Whirlwind");
        //CStringA symAsString = sym->stringdata->operator Common::CStringA();
        

        for (int i = 0; i < 3; i++) {
            menu_View->CreateText(1300 + 1500, 1300 + 450 + 225 * i, symAsString);
        }
        zSTRING Menu_STR_Value = (zSTRING)player->attribute[NPC_ATR_STRENGTH];
        menu_View->CreateText(menu_View->vposx, menu_View->vposy, "Si³a: " + Menu_STR_Value);
    }



    int NewStatsMenu_Show_External() {
        menu_View->Open();
        menu_View->InsertBack("NewStatusMenu_Background.tga");    //Put texture in Gothic II\_work\data\Textures
        NewStatsMenu_ShowPlayerStats();
        isMenuOpen = true;
        CursorStatic::Show();
        CraftingView::CallFuncForEachChild(CraftingView::childs.root, &zCView::Open);
        player->attribute[NPC_ATR_STRENGTH] += 1;
        zCPar_Symbol* sym = parser->GetSymbol("UseItPo_Mana_01");
        int ind = parser->GetIndex("UseItPo_Mana_01");
        int currentFunc = parser->GetCurrentFunc();
        zCPar_Symbol* currentFunc_sym = parser->GetSymbol(currentFunc);
        menu_View->CreateText(menu_View->vposx, menu_View->vposy + 400, "sym.single_intdata: " + (zSTRING)sym->single_intdata);
        menu_View->CreateText(menu_View->vposx, menu_View->vposy + 400, "ind: " + (zSTRING)ind);
        menu_View->CreateText(menu_View->vposx, menu_View->vposy + 600, "currentFunc: " + (zSTRING)currentFunc);
        menu_View->CreateText(menu_View->vposx, menu_View->vposy + 800, "currentFunc_Name: " + currentFunc_sym->name);

        return 0;
    }

    int NewStatsMenu_Hide_External() {
        menu_View->Close();
        isMenuOpen = false;
        //screen->RemoveItem(menu_View);
        //CursorStatic::Hide();
        delete button1; button1 = nullptr;      //if I use 'delete' without assigning nullptr to the variable, the game freezes on game exit
        CraftingView::CallFuncForEachChild(CraftingView::childs.root, &zCView::Close);
        return 0;
    }


    

    void NewStatsMenu_Show() {
        menu_View->Open();                                  //Using this, there's a short animation of the texture being enlarged when menu opens
        //screen->InsertItem(menu_View);                        //Using this, menu appears instantly, as in original Gothic
        //menu_View->SetColor(zCOLOR(255, 255, 255, 0));
        menu_View->InsertBack("NewStatusMenu_Background.tga");    //Put texture in Gothic II\_work\data\Textures
        NewStatsMenu_ShowPlayerStats();
        isMenuOpen = true;
        CursorStatic::Show();
        //button1 = new Menu_Button();
        //CraftingView::ShowChildRecursive(CraftingView::childs.root);
        //CraftingView::CallFuncForEachChild(CraftingView::childs.root, &zCView::Open);
        //zCPar_Symbol* sym = parser->GetSymbol("UseItPo_Mana_01");
        //int ind = parser->GetIndex("UseItPo_Mana_01");
        //int currentFunc = parser->GetCurrentFunc();
        //zCPar_Symbol* sym2 = parser->GetSymbol("UseItPo_Mana_01");

        
        

        
    }

    void NewStatsMenu_Hide() {
        menu_View->Close();
        isMenuOpen = false;
        //screen->RemoveItem(menu_View);
        //CursorStatic::Hide();
        delete button1; button1 = nullptr;      //if I use 'delete' without assigning nullptr to the variable, the game freezes on game exit
        CraftingView::CallFuncForEachChild(CraftingView::childs.root, &zCView::Close);


    }

    void NewStatsMenu_ChangeColor() {
        //renderView->InsertBack("white");
        zCOLOR* color = new zCOLOR();
        color->SetRGB(255, 120, 15);
        menu_View->SetFontColor(*color);
    }
    

    

    //I don't know why this function is used for the hook, but it apparently works (e.g. oCMenu_Status::InitForDisplay does not work)
    //void __thiscall oCNpc::OpenScreen_Status(void) - what about this?
    void __fastcall oCMenu_Status_SetLearnPoints_Hook(oCMenu_Status* _this, void*, unsigned long number);
    auto Hook_oCMenu_Status_SetLearnPoints_Hook = CreateHookByName(&oCMenu_Status::SetLearnPoints, &oCMenu_Status_SetLearnPoints_Hook, Hook_Auto);
    void __fastcall oCMenu_Status_SetLearnPoints_Hook(oCMenu_Status* _this, void* p0, unsigned long number) {
        NewStatsMenu_Show();
    }


    //This hooks the function that shows the status menu, so it doesn't show at all - that means the game is not paused, and you need to 
    //add some checks if the menu is open (because from the game perspective it isn't, as it never opens) and think how to close it
    /*void __fastcall oCMenu_Status_SetLearnPoints_Hook(oCStatusScreen* _this, void*);
    auto Hook_oCMenu_Status_SetLearnPoints_Hook = CreateHookByName(&oCStatusScreen::Show, &oCMenu_Status_SetLearnPoints_Hook, Hook_Auto);
    void __fastcall oCMenu_Status_SetLearnPoints_Hook(oCStatusScreen* _this, void* p0) {
        if (!isMenuOpen) NewStatsMenu_Show();
    }*/

    void __fastcall zCMenu_ScreenDone_Hook(zCMenu* _this, void*);
    auto Hook_zCMenu_ScreenDone_Hook = CreateHookByName(&zCMenu::ScreenDone, &zCMenu_ScreenDone_Hook, Hook_Auto);
    void __fastcall zCMenu_ScreenDone_Hook(zCMenu* _this, void* p0) {
        NewStatsMenu_Hide();
        Hook_zCMenu_ScreenDone_Hook(_this, p0); //This calls the original function - without it, other menus never close
    }

    
    
    
    

    


    


    //virtual int __thiscall CGameManager::HandleEvent(int)
    //virtual int __thiscall oCGame::HandleEvent(int)
    //virtual int __thiscall zCInputCallback::HandleEvent(int)
    /*int __fastcall TestEndDialog_Hook2(zCInputCallback* _this, void*, int key);
    auto Hook_TestEndDialog_Hook2 = CreateHookByName(&zCInputCallback::HandleEvent, &TestEndDialog_Hook2, Hook_Auto);
    int __fastcall TestEndDialog_Hook2(zCInputCallback* _this, void* p0, int key) {
        if (key == KEY_S) {
            messages_View->Open();
            messages_View->CreateText(100, 800, "KEY_S: ");
        }
        else if (key == KEY_W) {
            messages_View->Open();
            messages_View->CreateText(100, 800, "KEY_W: ");
        }
        return 1;
    }*/


    

}