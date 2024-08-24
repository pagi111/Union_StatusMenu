// Supported with union (c) 2020 Union team
// Union HEADER file

namespace GOTHIC_ENGINE {
    enum RECIPETYPE {
        ALCHEMY,
        SMITH
    };

    
    class C_RECIPE {
    public:
        //Fields that match the ones in Daedalus
        zSTRING 	name_id;		        //Recipe id name - must be identical as instance name
        int 	    known_recipes_index;    //Matters in Daedalus and here it's the order in which to display the recipes
        RECIPETYPE  type;                   //RECIPETYPE::ALCHEMY, ::SMITH, etc.
        int 	    level;                  //Level of the skill required to use learn this recipe
        int	        create_item;            //Instance of the item to create (as int) - from scripts
        int 	    req_itm[9];		        //Instances of required items
        int 	    req_qty[9];             //Number of each item required
        zSTRING     req_items;              //Required items with their numbers (syntax: itemName1: quantity1 | itemName2 : quantity2 |itemName3: quantity3 ... - spaces don't matter)

        //Additional fields (not present in Daedalus)
        oCItem*     item;                   //Pointer to the actual item to create (created here in the constructor, not by scripts)
        Map<zSTRING, int>* requirements = new Map<zSTRING, int>();
        

        C_RECIPE(zSTRING recipeName);
        C_RECIPE(int recipeInstance);
    };
    
    Array<int>* knownRecipes_Instances = new Array<int>();
    Array<C_RECIPE>* knownRecipes = new Array<C_RECIPE>();



    class Menu_Text {
    public:
        zSTRING text;
        zCView* view = nullptr;
        int posX = 0;
        int posY = 0;
        zCFont* font = nullptr;
        int fontHeight = 0;
        zCOLOR fontColor = zCOLOR(255, 255, 255, 255);

        Menu_Text(zSTRING _text) {
            text = _text;
        }
        Menu_Text(zSTRING _text, int _posX, int _posY) {
            posX = _posX;
            posY = _posY;
        }
        Menu_Text(zSTRING _text, int _posX, int _posY, zCOLOR _fontColor) {
            fontColor = _fontColor;
        }
    };

    

    class Menu_Button {
    public:
        zSTRING name;
        zCView* view = new zCView();
        int& posX = view->vposx;
        int& posY = view->vposy;
        int& sizeX = view->vsizex;
        int& sizeY = view->vsizey;
        
        inline static int defaultSizeX = 400;
        inline static int defaultSizeY = 500;
        inline static int defaultTextOffsetX = 3500;    //Each zCView dimension is always 8192, so 4096 would be in the middle of the zCView
        inline static int defaultTextOffsetY = 2500;    //Each zCView dimension is always 8192, so 4096 would be in the middle of the zCView
        inline static zSTRING defaultBackTex = "AlchemyScreen_Tab2_Dark.tga";
        
        bool hovered = false, clicked = false, selected = false;

        Menu_Button();
        Menu_Button(int posX, int posY, zSTRING text, zCOLOR textColor);
        Menu_Button(int posX, int posY, int sizeX, int sizeY, zSTRING text, zCOLOR textColor);


    };
    
    bool IsCursorHovering(Menu_Button* btn) {
        if (CursorStatic::x > btn->posX && CursorStatic::x < btn->posX + btn->sizeX &&
            CursorStatic::y > btn->posY && CursorStatic::y < btn->posY + btn->sizeY) {
            btn->hovered = true;
            return true;
        }
        else {
            btn->hovered = false;
            return false;
        }
    }


    class CraftingView {
    public:

        inline static zCView* view = new zCView(0, 0, 8192, 8192);
        inline static zList<zCView>& childs = view->childs;
        inline static Array<Menu_Button*> childsArray;
        //inline static Map<int, zSTRING> map;

        inline static bool isOpen = false;
        inline static bool potBtnCreated = false;
        inline static zCWorld* renderItem_World;
        inline static int selectedRecipeInd = -1;

        static void Init();
        static void RenderButton(int posX, int posY, int sizeX, int sizeY, zSTRING text, zCOLOR textColor);
        static void RenderButton(Menu_Button* btn);
        static void RenderText(zSTRING text, int posX, int posY, zCOLOR color, zSTRING font, int fontHeight);
        static void RenderItem(oCItem* item, int posX, int posY, int sizeX, int sizeY);
        static void RenderItem(zSTRING itemName, int posX, int posY, int sizeX, int sizeY);
        static void RenderItem(int itemInstance, int posX, int posY, int sizeX, int sizeY);
        static void Update_Alchemy();
        static void Update();
        static void Open_Alchemy();
        static void Close();

        //This function calls a function (by function pointer) for each child of this zCView (in zCView::childs)
        //Arguments:
        //  child:            this->childs.root                                     - it's the first child in the list
        //  funcPtr:        &zCView::Open, &zCView::Close, etc.     - with exactly such syntax
        static void CallFuncForEachChild(zCView* child, void(zCView::* funcPtr)());



    };

    //This class serves as a View to display lines of text.
    //Create a Menu_TextView object, then use AddText - use it multiple times to add multiple lines of text. Then use Render to render all.
    class Menu_TextView {
    public:
        zCView* textView = new zCView();
        zCView* mainView = nullptr;

        Menu_TextView(zCView* _mainView) {
            mainView = _mainView;
            textView->SetPos(0, 0);
            textView->SetSize(8192, 8192);
        }

        //The last arg allows for changing the font size - note, however, that this changes the height of the font permanently.
        //The new height will be used for this font even if it is used somewhere else. 
        //It's also not possible to add each text with the same font but different height as the texts get rendered in another method and with the font height that was specified as last.
        zCViewText* AddText(zSTRING text, int posX, int posY, zCOLOR color = zCOLOR(255, 255, 255, 255), zSTRING font = "Font_Default.tga", int fontHeight = 0) {
            zCViewText* t = new zCViewText();
            zCFont* f;  

            //If I wanted to load a new font, I could use font.LoadFontTexture(font_name); - but this takes a lot of time
            //And should not be used in an every-frame function. But usually it's better to use zfontman (global font manager) to search fonts, like below.
            f = zfontman->GetFont(zfontman->SearchFont(font)); //Font cannot be set in Game_Init() - it's too early and it can't be found correctly
            if (fontHeight != 0) { f->height = fontHeight; }

            t->font = f;
            t->posx = posX;
            t->posy = posY;
            t->text = text;
            t->color = color;
            t->colored = true;

            //With Font_Default.tga (which is the same as Font_Old_10_White), zCOLOR(255, 255, 255) means Gothic golden font color, not white.
            //With regular font this golden color is zCOLOR(200, 184, 152)
            textView->textLines.Insert(t);

            return t;
        }

        //By default Render deletes all zCViewText* objects in textView->textLines. This is because those object are created in AddText method.
        //Disable it, if you have created some zCViewText* objects elsewhere (not in an every-frame function) and you manually added them to textView->textLines, so that they don't get deleted.
        void Render(bool deleteTextLinesObjects = true) {
            mainView->InsertItem(textView);
            textView->Blit();
            mainView->RemoveItem(textView);

            for (int i = 0; i < textView->textLines.GetNum(); i++) {
                delete textView->textLines.Get(i);
            }
            textView->textLines.DeleteList();
        }
    };

    class CBook : public zCInputCallback {
    public:
        CBook();
        virtual int HandleEvent(int);
    };
    CBook::CBook()
    {
        SetEnableHandleEvent(TRUE);
        SetHandleEventTop();
    }
    int CBook::HandleEvent(int key)
    {
        if (zinput->IsBindedToggled(GAME_DOWN, key)) //||
            //(key == MOUSE_BUTTONRIGHT && zinput->KeyToggled(MOUSE_BUTTONRIGHT)))
        {
            return TRUE;
        }
        else if (zinput->IsBinded(GAME_LEFT, key) || zinput->IsBinded(GAME_STRAFELEFT, key))
        {
            //ChangePage(-2);
            return TRUE;
        }
        else if (zinput->IsBinded(GAME_RIGHT, key) || zinput->IsBinded(GAME_STRAFERIGHT, key))
        {
            //ChangePage(2);
            return TRUE;
        }

        return TRUE;
    }




    //oCMobInter::EndInteraction(class oCNpc *,int)             //Called when interaction with a Mob ends (e.g. ending smithing/alchemy, etc. - but also Mobs that do not display any dialogs)
    //oCMobInter::OnEndStateChange(class oCNpc *,int,int)       //When Mob goes into a new state (like when you use the alchemy table, or stop using it)
    //oCMobInter::OnBeginStateChange(class oCNpc *,int,int)     //Same as OnEndStateChange, but before the event, not after
        //- but simply returning from the function changes nothing (PC still goes into animation of preparing potions or quits using the table), so I'm not sure about it
    //oCMobInter::StopInteraction(class oCNpc *)                //Called a bit AFTER oCMobInter::EndInteraction
    //oCNpc::EV_StopProcessInfos(class oCMsgConversation *)     //Doesn't seem to be called in dialog/MOB interaction

    //oCInformationManager::OnExit(void)
    //oCMobInter::SendEndInteraction(class oCNpc *,int,int)     //Doesn't seem to be called in dialog/MOB interaction
    //oCMobInter::CheckStateChange(class oCNpc *)
    //oCMobInter::CallOnStateFunc(class oCNpc *,int)
    //oCMobInter::SendCallOnStateFunc(class oCNpc *,int)
    //void __thiscall oCNpc::SetMovLock(int)                    //In menus and while interacting with Mobs (player can't move), but it's weird, as when interacting this is called only after the interacting animation has been played, even though player can't move during the animation as well.
    //__thiscall zCResource::LockStateChange(void)
    //__thiscall zCResource::UnlockStateChange(void)
    //void __thiscall zCAIPlayer::BeginStateSwitch(void)
    //oCMobInter::InterruptInteraction(class oCNpc *)                               //As the name suggests, this is only when the interaction is interrupted (e.g. NPC is doing something, and PC start dialog).
    //void __thiscall oCNpc::SetInteractMob(class oCMobInter *)                     //When you start (before animation even plays) interaction, or after the whole finishing animation - then oCMobInter becomes nullptr)
    //void __thiscall oCMobInter::StartStateChange(class oCNpc *,int,int)
    //void __thiscall oCMobInter::SendStateChange(int,int)
    //void __thiscall oCMobInter::StartTransitionAniNpc(class oCNpc*, class zSTRING const&)
    //void __thiscall oCMobInter::SetTempState(int)
    //void __thiscall oCMobInter::SetMobBodyState(class oCNpc *)
    //void __thiscall oCMobInter::OnUntrigger(class zCVob *,class zCVob *)
    //virtual void __thiscall oCMobInter::OnMessage(class zCEventMessage *,class zCVob *)

    zCView* craftMessages_View = new zCView(0, 0, 8192, 8192);
    bool allowEndInteraction = false;
    //This function ends interaction of the NPC with the current Mob - unfortunately native oCMobInter::EndInteraction does not work this way. 
    //Arguments: oCNpc* = npc that is currently interacting with a mob
    void EndInteractionWithMob(oCNpc* npc) {
        //oCMsgManipulate* msgManipulate = new oCMsgManipulate();                       //We need this pointer for the actual end interaction function
        //msgManipulate->targetVob = npc->interactMob;                                  //->targetVob seems to be the object currently in interaction
        //msgManipulate->subType = oCMsgManipulate::TManipulateSubType::EV_DROPMOB;     //EV_DROPMOB seems to end interaction
        
        if (npc == nullptr || npc->interactMob == nullptr) { return; }
        allowEndInteraction = true;
        oCMsgManipulate* msgManipulate = new oCMsgManipulate(oCMsgManipulate::TManipulateSubType::EV_DROPMOB, npc->interactMob); //One-liner
        npc->EV_UseMob(msgManipulate);                                                  //This is the actual function we need to use
        allowEndInteraction = false;
    }



    //This hook disallows ending interaction with a MobInter (e.g. by pressing 's' key or clicking "END"), until the global variable is changed to true. 
    //For Mobs with no dialogs (e.g. bookstand), simply nothing happens after pressing 's'. 
    //For Mobs with dialogs (e.g. alchemy table), after clicking the "END", the dialog disappears, but interaction doesn't stop and animation still plays.
    //In both cases we need to manually end the interaction by setting the variable to true and calling end animation (see: EndInteractionWithMob function above)
    void __fastcall EndInteraction_Hook(oCMobInter* _this, void*, zCEventMessage* evntMsg, zCVob* vob1);
    auto Hook_EndInteraction_Hook = CreateHookByName(&oCMobInter::OnMessage, &EndInteraction_Hook, Hook_Auto);
    void __fastcall EndInteraction_Hook(oCMobInter* _this, void* p0, zCEventMessage* evntMsg, zCVob* vob1) {
        /* --------------- Explanation of how oCMobInter::OnMessage works below : --------------- */
        //IMPORTANT: the below function isn't called when PC hasn't got required item (steel with anvil, etc.) or when Mob is to far away
        //So this is definitely not the first function that plays when activating an interactive Mob
        //Debugging the function: oCMobInter::OnMessage
        //With the following code:
        // if (player->interactMob == _this) {
        //  NewStatsMenu_Show();    //Breakpoint set here!
        //}
        //Hook_EndInteraction_Hook(_this, p0, evntMsg, vob1);
        //I was trying to see what changes when PC starts/ends interaction. zCEventMessage->subType definitely changes, but it's not enough
        //to decide what the state is, based on that only. Thus, I observed that some fields in oCMobInter change: state, state_target, npcStateAni.
        //But some stay always the same: state_num = 1, mobStateAni = -1 (some others too - but probably not important here).
        //IMPORTANT: Based on the info below here are my conclusions:
        //zCEventMessage->subType 1 - is like an instruction to start/end an interaction
        //zCEventMessage->subType 5 - sort of a transition instruction, as it always tells to go from one 'state' to another 'state_target'
        //zCEventMessage->subType 2 - this means the definitive end of the interaction as it is always played last and only when ending the interaction
        // 
        //With bookstand (no Mob dialog) - zCEventMessage->subType equals: 
        //A) 1 - after the first ani (starting to read - so already after PC walks to the bookstand) has been played; 
        //  With the above: oCMobInter::state = 0, state_target = 0, npcStateAni = 97
        //B) 5 - some time (enough to display game window again when debugging) after point A (after starting the interaction), when actual reading animation is being played 
        //  With the above: oCMobInter::state = 0, state_target = 1, npcStateAni = 98
        //C) 1 - also right after pressing 's' to step away from the bookstand, before the animation even starts
        //  With the above: oCMobInter::state = 1, state_target = 1, npcStateAni = 98
        //D) 5 - immediately after point C
        //  With the above: oCMobInter::state = 1, state_target = 0, npcStateAni = 97
        //E) 2 - shortly (~0.5 sec) after point D
        //  With the above: oCMobInter::state = 0, state_target = 0, npcStateAni = -1
        //
        //With alchemy table (alchemy dialog) - zCEventMessage->subType equals: 
        //A) 1 - after the first ani (starting to alchemy - so already after PC walks to the alchemy table) has been played; 
        //  With the above: oCMobInter::state = 0, state_target = 0, npcStateAni = 216
        //B) 5 - some time (enough to display game window again when debugging) after point A (after starting the interaction), when actual alchemy animation is being played 
        //  With the above: oCMobInter::state = 0, state_target = 1, npcStateAni = 217
        //C) 1 - right after clicking 'END DIALOG', before the animation even starts
        //  With the above: oCMobInter::state = 1, state_target = 1, npcStateAni = 217
        //D) 5 - NOT immediately (enough to display game window again when debugging) after point C
        //  With the above: oCMobInter::state = 1, state_target = 0, npcStateAni = 216
        //E) 2 - shortly (~0.5 sec) after point D
        //  With the above: oCMobInter::state = 0, state_target = 0, npcStateAni = -1
        //
        //With the forge (no Mob dialog) - zCEventMessage->subType equals: 
        //A) 1 - after camera view changes (focus on the forge); 
        //  With the above: oCMobInter::state = 0, state_target = 0, npcStateAni = 118
        //B) this point is absent with the forge, so zCEventMessage->subType never equals 5
        //C) 1 - right after pressing 's' to step away from the forge, before the animation even starts
        //  With the above: oCMobInter::state = 1, state_target = 1, npcStateAni = 119
        //D) this point is absent with the forge, so zCEventMessage->subType never equals 5
        //E) 2 - shortly (~0.5 sec) after point C
        //  With the above: oCMobInter::state = 0, state_target = 0, npcStateAni = -1
        //
        //With the anvil (smithing dialog) - zCEventMessage->subType equals: 
        //A) 1 - after camera view changes (focus on the anvil) and PC looks at the anvil, but hasn't taken the hammer yet, and the smithing ani hasn't started; 
        //  With the above: oCMobInter::state = 0, state_target = 0, npcStateAni = 114
        //B) 5 - shortly (NOT enough to display game window again when debugging) after point A (after starting the interaction), when actual alchemy animation is being played 
        //  With the above: oCMobInter::state = 0, state_target = 1, npcStateAni = 115
        //C) 1 - right after clicking 'END DIALOG', before the animation even starts
        //  With the above: oCMobInter::state = 1, state_target = 1, npcStateAni = 115
        //D) 5 - immediately after point C
        //  With the above: oCMobInter::state = 1, state_target = 0, npcStateAni = 114
        //E) 2 - shortly (~0.5 sec) after point D
        //  With the above: oCMobInter::state = 0, state_target = 0, npcStateAni = -1

        if (player->interactMob == _this) {
            craftMessages_View->Open();
            craftMessages_View->CreateText(100, 600, "zCEventMessage->subType: " + (zSTRING)evntMsg->subType);
            craftMessages_View->CreateText(100, 800, "oCMobInter::state: " + (zSTRING)_this->state);
            craftMessages_View->CreateText(100, 1000, "oCMobInter::state_target: " + (zSTRING)_this->state_target);
            craftMessages_View->CreateText(100, 1200, "oCMobInter::npcStateAni: " + (zSTRING)_this->npcStateAni);
            craftMessages_View->CreateText(100, 1400, "oCMobInter::GetInstanceName: " + (zSTRING)_this->GetInstanceName()); //Nothing in this case
            craftMessages_View->CreateText(100, 1600, "oCMobInter::GetName: " + (zSTRING)_this->GetName());     //Translation/Display name, e.g. "Ose³ka"
            craftMessages_View->CreateText(100, 1800, "oCMobInter::GetObjectName: " + (zSTRING)_this->GetObjectName()); //Nothing in this case
            craftMessages_View->CreateText(100, 2000, "oCMobInter::GetScemeName: " + (zSTRING)_this->GetScemeName());   //ID name of the action(?), e.g. "BSSHARP"
            craftMessages_View->CreateText(100, 2200, "oCMobInter::name: " + (zSTRING)_this->name);             //ID name of the Mob, e.g. "MOBNAME_GRINDSTONE"

            //If below conditions are met, ending the interaction begins (it's right after pressing 's' or clicking "END DIALOG")
            if (!allowEndInteraction && evntMsg->subType == 1 && _this->state == 1 && _this->state_target == 1) {
                return;
            }
        }
        Hook_EndInteraction_Hook(_this, p0, evntMsg, vob1);
    }

    //void __fastcall TestEndDialog_Hook(oCMobInter* _this, void*, oCNpc* npc, int num1, int num2);
    //auto Hook_TestEndDialog_Hook = CreateHookByName(&oCMobInter::StartStateChange, &TestEndDialog_Hook, Hook_Auto);
    //void __fastcall TestEndDialog_Hook(oCMobInter* _this, void* p0, oCNpc* npc, int num1, int num2) {
    //    //NewStatsMenu_Show();
    //    if (npc == player) {
    //        //NewStatsMenu_Show();
    //        //Hook_TestEndDialog_Hook(_this, p0, npc);
    //    }
    //    else {
    //        Hook_TestEndDialog_Hook(_this, p0, npc, num1, num2); 
    //    }
    //}


    //int count_anis = 0;
    //oCLogTopic* topic = new oCLogTopic();   //Topic musi byæ stworzony za pomoc¹ new
    //void __fastcall TestEndDialog_Hook(oCMobInter* _this, void*, oCNpc* npc, zSTRING const& num1);
    //auto Hook_TestEndDialog_Hook = CreateHookByName(&oCMobInter::StartTransitionAniNpc, &TestEndDialog_Hook, Hook_Auto);
    //void __fastcall TestEndDialog_Hook(oCMobInter* _this, void* p0, oCNpc* npc, zSTRING const& num1) {
    //    //NewStatsMenu_Show();
    //    if (npc == player) {
    //        //NewStatsMenu_Show();
    //        
    //        if (count_anis == 0) {
    //            oCLogManager::GetLogManager().m_lstTopics.DeleteList(); //Tak mo¿na usun¹æ wszystkie wpisy
    //            
    //            topic->m_enuSection = oCLogTopic::zELogSection_Notes;   //Sekcja: Notes/Missions/All
    //            topic->m_strDescription = "Anis names";
    //            oCLogManager::GetLogManager().m_lstTopics.Insert(topic);    //Dodajemy temat do listy
    //        }
    //        count_anis++;
    //        topic->AddEntry(num1 + " - " + (zSTRING)count_anis);    //Tak dodajemy kolejne wpisy
    //        if (num1 != "T_BOOK_S1_2_S0") {
    //            Hook_TestEndDialog_Hook(_this, p0, npc, num1);
    //        }
    //        else {
    //            Hook_TestEndDialog_Hook(_this, p0, npc, num1);
    //        }
    //    }
    //    else {
    //        Hook_TestEndDialog_Hook(_this, p0, npc, num1); //This calls the original function - without it, other menus never close
    //    }
    //}

}