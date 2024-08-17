// Supported with union (c) 2020 Union team
// Union SOURCE file
#include <cmath>
namespace GOTHIC_ENGINE {
    int GAPX = 100;
    int GAPY = 200;
    int navBar_x = 180;
    int navBar_y = 500;
    int btnSizeX = Menu_Button::defaultSizeX;
    int btnSizeY = Menu_Button::defaultSizeY;
    Menu_TextView* txtV_ItemStats;
    zCOLOR col_Orange = zCOLOR(255, 100, 0);        //Ingredient name on label
    zCOLOR col_RedDark = zCOLOR(167, 11, 11);       //If not enough ingredients
    zCOLOR col_Green = zCOLOR(22, 114, 22);         //If enough ingredients
    zCOLOR col_BrownDark = zCOLOR(90, 51, 39);      //Item Name
    zCOLOR col_BrownLight = zCOLOR(120, 71, 50);    //Item Stats

    C_RECIPE::C_RECIPE(zSTRING recipeName) {
        int recipeInd = parser->GetIndex(recipeName);   //Get instance index from parser
        if (recipeInd > 0) {                            //If found...
            parser->CreateInstance(recipeInd, this);    //...create an instance and assign it to 'this'
            item = new oCItem(create_item, 1);            //then create a new oCItem object
        }
    }
    C_RECIPE::C_RECIPE(int recipeInstance) {
        if (recipeInstance > 0) {                           //If found...
            parser->CreateInstance(recipeInstance, this);   //...create an instance and assign it to 'this'
            item = new oCItem(create_item, 1);              //then create a new oCItem object
        }
    }

    Menu_Button::Menu_Button() {
        view->InsertBack(defaultBackTex);
        view->SetSize(defaultSizeX, defaultSizeY);
        view->SetPos(0, 0);
    }
    Menu_Button::Menu_Button(int posX, int posY, zSTRING text = "", zCOLOR textColor = zCOLOR(255, 255, 255)) : Menu_Button::Menu_Button() {
        view->SetPos(posX, posY);
        view->CreateText(defaultTextOffsetX, defaultTextOffsetY, text, 50, textColor, 0, 1);
    }
    Menu_Button::Menu_Button(int posX, int posY, int sizeX, int sizeY, zSTRING text = "", zCOLOR textColor = zCOLOR(255, 255, 255)) : Menu_Button::Menu_Button(posX, posY, text, textColor) {
        view->SetSize(sizeX, sizeY);
    }

    void CraftingView::Init() {
        if (!renderItem_World) {
            renderItem_World = zfactory->CreateWorld();
        #if ENGINE == Engine_G2A
            renderItem_World->m_bIsInventoryWorld = TRUE;
        #endif
        }

        txtV_ItemStats = new Menu_TextView(CraftingView::view);
        

    }

    void CraftingView::RenderButton(Menu_Button* btn) {
        Menu_Button::defaultTextOffsetX = 2500;

        btn->view->SetPos(btn->posX, btn->posY);
        btn->view->SetSize(btn->sizeX, btn->sizeY);

        view->InsertItem(btn->view);
        btn->view->Blit();
        view->RemoveItem(btn->view);
    }
    void CraftingView::RenderButton(int posX, int posY, int sizeX, int sizeY, zSTRING text = "", zCOLOR textColor = zCOLOR(255, 255, 255)) {
        Menu_Button* btn = new Menu_Button(posX, posY, sizeX, sizeY, text, textColor);

        RenderButton(btn);

        delete btn; btn = nullptr;
    }
    
    void CraftingView::RenderText(zSTRING text, int posX, int posY, zCOLOR color = zCOLOR(255, 255, 255, 255), zSTRING font = "Font_Default.tga", int fontHeight = 0) {
        zCView v;
        zCViewText t;
        zCFont* f;

        v.SetPos(0, 0);
        v.SetSize(8192, 8192);
        v.SetFontColor(color); //95, 30, 30  
            //With Font_Default.tga (which is the same as Font_Old_10_White), zCOLOR(255, 255, 255) means Gothic golden font color, not white.
            //With regular font this golden color is zCOLOR(200, 184, 152) or (248, 220, 168)
        
        v.textLines.Insert(&t);

        //If I wanted to load a new font, I could use font.LoadFontTexture(font_name); - but this takes a lot of time
        //And should not be used in an every-frame function. But usually it's better to use zfontman (global font manager) to search fonts, like below.
        f = zfontman->GetFont(zfontman->SearchFont(font));
        if (fontHeight != 0) { f->height = fontHeight; }
        
        t.font = f;
        t.posx = posX;
        t.posy = posY;
        t.text = text;
        //t.color = zCOLOR(95, 30, 30);


        view->InsertItem(&v);
        v.Blit();
        view->RemoveItem(&v);

    }

    void CraftingView::RenderItem(oCItem* item, int posX, int posY, int sizeX, int sizeY)
    {
        int prevRefCount = item->refCtr;
        zCView itemRenderView;
        itemRenderView.SetPos(posX, posY);
        itemRenderView.SetSize(sizeX, sizeY);

        view->InsertItem(&itemRenderView);
        itemRenderView.Blit();
        oCItem::SetLightingSwell(FALSE);
        //This function increases item->refCtr by 2, so it's necessary to item->Release() two times after this function's call.
        //But apparently not always - it happened once (so far) that it didn't raise refCtr at all - it depends on where the function is called.
        item->RenderItem(renderItem_World, &itemRenderView, 0.0f);  
        //item->refCtr;   //This shows how many refs there are to the object - every obj->Release() lowers it by 1 and when it reaches 0, item is deleted from the memory.
        oCItem::SetLightingSwell(TRUE);
        //item->refCtr;
        view->RemoveItem(&itemRenderView);

        int currentRefCount = item->refCtr;
        for (int i = 0; i < currentRefCount - prevRefCount; i++) {
            item->Release(); //Since RenderItem sometimes increases refCtr, sometimes not, we need to calculate by how much it has raised
        }
    }
    void CraftingView::RenderItem(zSTRING itemName, int posX, int posY, int sizeX, int sizeY)
    {
        if (oCItem* item = new oCItem(itemName, 1)) {
            RenderItem(item, posX, posY, sizeX, sizeY);
            item->Release(); //This is for new oCItem
        }
    }
    void CraftingView::RenderItem(int itemInstance, int posX, int posY, int sizeX, int sizeY)
    {
        if (oCItem* item = new oCItem(itemInstance, 1)) {
            RenderItem(item, posX, posY, sizeX, sizeY);
            item->Release(); //This is for new oCItem
        }
    }

    void CraftingView::Update_Alchemy() {
        //Values for item rendering:
        int gapX = 350;             //Gap between items
        int itmBasePosX = 100;      //For recipe item (left side of the screen)
        int itmBasePosY = 1300;
        int ingrBasePosX = 6000;    //6000 //For ingredients (right side of the screen)
        int ingrBasePosY = 1800;    //5600
        int itmSizeX = 750;         //Size is the same for recipe item and ingredients (for now at least)
        int itmSizeY = 1100;
        //Values for rendering a box(button, background) for an item model:
        //Box will be a bit bigger than the model, but sometimes may be too small: PosX: + (sizeX * 0.18), PosY: no change, SizeX: sizeX / 1.5, SizeY: sizeY / 1.05
        //Box will fit the model a bit more tightly, so it may be too small: PosX: + (sizeX * 0.2), PosY: + (sizeY * 0.05), SizeX: sizeX / 1.7, SizeY: sizeY / 1.15
        //Best fit (a bit smaller than the model): PosX: + (sizeX * 0.30), PosY: + (sizeY * 0.15), SizeX: sizeX / 2.5, SizeY: sizeY / 1.25
        //Box smaller than the model and square in shape: PosX: + (sizeX * 0.28), PosY: + (sizeY * 0.28), SizeX: sizeX / 2.4, SizeY: sizeY / 2.0
        float boxPosXOffsetMult = 0.28;
        float boxPosYOffsetMult = 0.28; 
        float boxSizeXDivide = 2.4;
        float boxSizeYDivide = 2.0;
        //Values for displaying info about chosen item to craft:
        int itmNamePosX = 6300;     //6600
        int itmNamePosY = 750;     //200
        int itmStatsPosY = 3900;     //700
        int itmTextPosX = 6100;     //6500
        int itmCountPosX = 7600;    //7850
        int itmCenterX = 6725;
        int itmCenterY = 2175;
        int radius = 600;
        
        
        //Render item for each known recipe (left side of the screen)
        for (int i = 0; i < knownRecipes->GetNum(); i++) {
            Menu_Button::defaultBackTex = "AlchemyScreen_Item_Bg.tga";
            Menu_Button* btn = new Menu_Button(itmBasePosX + (itmSizeX * boxPosXOffsetMult) + gapX * i, itmBasePosY + (itmSizeY * boxPosYOffsetMult), itmSizeX / boxSizeXDivide, itmSizeY / boxSizeYDivide);
            //btn->name = "RecBtn_" + (zSTRING)(*knownRecipes_Instances->GetSafe(i));

            if (IsCursorHovering(btn) && zinput->GetMouseButtonPressedLeft() == 1) {
                selectedRecipeInd = i;
            }

            C_RECIPE* rec = knownRecipes->GetSafe(i);
            oCItem* itm = new oCItem(rec->create_item, 1);

            CraftingView::RenderButton(btn);
            CraftingView::RenderItem(itm, itmBasePosX + gapX * i, itmBasePosY, itmSizeX, itmSizeY); //Here RenderItem increases itm->RefCtr by 2
            itm->Release();
            delete btn; btn = nullptr;
        }

        //If a recipe from the left side of the screen is selected, render the right side (item model, stats, required ingredients).
        if (selectedRecipeInd >= 0) {
            C_RECIPE* selRec = knownRecipes->GetSafe(selectedRecipeInd);
            oCItem* selItm = new oCItem(selRec->create_item, 1);

            //Item name
            txtV_ItemStats->AddText(selItm->name, itmNamePosX, itmNamePosY, zCOLOR(90, 51, 39), "Font_Old_20_White_Hi.tga"); //(64, 36, 15)
            //Item stats (text[x] and count [x] - the same as item description from inventory)
            for (int i = 0; i < 6 /* itm->text.COUNT */; i++) {
                if (selItm->text[i].IsEmpty()) continue;
                int y = itmStatsPosY + 200 * i;
                txtV_ItemStats->AddText(selItm->text[i], itmTextPosX, y, zCOLOR(120, 71, 50), "Font_Old_10_White_Hi.tga"); //(200, 184, 152)

                if (selItm->count[i] == 0) continue;
                txtV_ItemStats->AddText(selItm->count[i], itmCountPosX, y, zCOLOR(120, 71, 50), "Font_Old_10_White_Hi.tga"); //(200, 184, 152)
            }
            txtV_ItemStats->Render();
            //Item model
            CraftingView::RenderItem(selItm, 6300, 1700, itmSizeX * 1.7, itmSizeY * 1.7); //Here RenderItem does not increase itm->RefCtr
            selItm->Release();

            //Ingredients required for the recipe
            bool renderIngrLabel = false;
            float renderIngrLabelX = 0;
            float renderIngrLabelY = 0;
            zSTRING renderIngrLabelName = "";

            int i = 0;
            while (selRec->req_itm[i] > 0) {
                int instance = selRec->req_itm[i];
                //oCItem* item = new oCItem(requiredItems->GetSafe(i)->instanz, 1); //Same as above
                //RenderButton(ingrBasePosX + (itmSizeX * 0.30) + gapX * i, ingrBasePosY + (itmSizeY * 0.15), itmSizeX / 2.5, itmSizeY / 1.25);
                //RenderItem(instance, ingrBasePosX + gapX * i, ingrBasePosY, itmSizeX*0.6, itmSizeY*0.6);
                
                //Values for displaying required ingredients in a circle
                float ingrPosX = itmCenterX + radius * sin(3.14 + 6.28f / 9.0f * i);
                float ingrPosY = itmCenterY + radius * 1.78f * cos(3.14 + 6.28f / 9.0f * i);
                float ingrSizeX = itmSizeX * 0.6;
                float ingrSizeY = itmSizeY * 0.6;
                float bgSizeMult = 0.90f;
                float bgPosOffsetX = ingrSizeX * (1 - bgSizeMult) / 2.0f; //If we make bg smaller, then we need to add to the pos the difference between ingr size and bg size and then divide it by 2
                float bgPosOffsetY = ingrSizeY * (1 - bgSizeMult) / 2.0f;

                Menu_Button::defaultBackTex = "AlchemyScreen_Ingr_Bg.tga";
                Menu_Button* ingrBtn = new Menu_Button(ingrPosX + bgPosOffsetX, ingrPosY + bgPosOffsetY, ingrSizeX * bgSizeMult, ingrSizeY * bgSizeMult);

                player->inventory2.UnpackAllItems();
                //auto it = player->IsInInv(ingr->objectName, 1);
                int ingrNumber1 = player->inventory2.GetAmount(instance);
                int ingrNumber2 = selRec->req_qty[i];
                zCOLOR col1 = ingrNumber1 < ingrNumber2 ? zCOLOR(167, 11, 11) : zCOLOR(22, 114, 22);
                zCOLOR col2 = zCOLOR(120, 71, 50); //(120, 71, 50) //179, 139, 92
                
                ingrBtn->view->CreateText(1500, 5000 + 800, ingrNumber1, 0, col1, 0, 1);   //Left-right: 1500, 5000; UppperRightCircle: 5200, 0; LeftUp-BottomRight: 1000, 0
                ingrBtn->view->CreateText(5500, 5000 + 800, ingrNumber2, 0, col2, 0, 1); //Left-right: 5500, 5000; UppperRightCircle: 7000, 2000; LeftUp-BottomRight: 7000, 4000
                //ingrBtn->view->CreateText(6100, 1000, "/ ", 0, zCOLOR(179, 139, 92), 0, 1);
                RenderButton(ingrBtn);

                oCItem* ingr = new oCItem(instance, 1);
                RenderItem(ingr, ingrPosX, ingrPosY, ingrSizeX, ingrSizeY);
                
                //If cursor is hovering over an ingredient, update the vars to render ingr name later below
                if (IsCursorHovering(ingrBtn)) {
                    renderIngrLabel = true;
                    //Option 1
                    //renderIngrLabelX = ingrPosX + bgPosOffsetX + ingrSizeX / 2;
                    //int offsetXIfOffScreen = renderIngrLabelX + 1250 - 8192;
                    //renderIngrLabelX = offsetXIfOffScreen > 0 ? renderIngrLabelX - offsetXIfOffScreen : renderIngrLabelX;
                    //Option 2
                    renderIngrLabelX = ingrPosX + bgPosOffsetY + ingrSizeX / 2 - 1250 / 2;
                    renderIngrLabelY = ingrPosY + bgPosOffsetY - ingrSizeY / 2;
                    renderIngrLabelName = ingr->description;
                }
                i++;
                ingr->Release();
                delete ingrBtn;
            }
            //Display ingredient name when hovering
            if (renderIngrLabel) {
                Menu_Button::defaultBackTex = "AlchemyScreen_Ingr_NameLabel.tga";
                Menu_Button::defaultTextOffsetX = 500;
                Menu_Button::defaultTextOffsetY = 2500;
                RenderButton(renderIngrLabelX, renderIngrLabelY, 1250, 480, renderIngrLabelName, zCOLOR(255, 100, 0));
            }
            
        }
    }


    void FillReqsFromString(C_RECIPE* recipe) {
        Array<zSTRING>* reqs = new Array<zSTRING>();
        SplitString(recipe->req_items, "|", *reqs);
        for (int i = 0; i < reqs->GetNum(); i++) {
            TrimString(*reqs->GetSafe(i));
            Array<zSTRING> req_itmNum;
            SplitString(reqs->GetSafe(i), ":", req_itmNum);
            zSTRING itm = req_itmNum.GetSafe(0);
            zSTRING numAsStr = req_itmNum.GetSafe(1);
            TrimString(itm);
            TrimString(numAsStr);

            int num = numAsStr.ToInt32();
            recipe->requirements->Insert(itm, num);

            //If I want to insert from Strings to req_itm and req_qty
            //int instance = parser->GetIndex(itm);
            //recipe->req_itm[i] = instance;
            //recipe->req_qty[i] = num;
        }
    }

    void CraftingView::Update() {
        if (!isOpen) return;
        /*if (!ogame || ogame->IsOnPause()) {
            return;
        }*/

        screen->InsertItem(view);
        view->Blit();

        Update_Alchemy();   //TO DO: Add sth like if CraftingType == Alchemy, else if CraftingType == Smithing, etc.

        screen->RemoveItem(view);
    }

    void CraftingView::Open_Alchemy() {
        isOpen = true;
        view->InsertBack("AlchemyScreen_Background_New.tga");

        screen->InsertItem(view);
        view->Blit();

        zCPar_Symbol* knownRecipes_sym = parser->GetSymbol("KnownRecipes");
        int knownRecipes_max = parser->GetSymbol("KnownRecipes_Max")->single_intdata; 
        int* knownRecipes_parser = (int*)knownRecipes_sym->adr;                         //Address of the array
        for (int i = 0; i < knownRecipes_sym->ele; i++) {
            knownRecipes_Instances->InsertEnd(knownRecipes_parser[i]);
            knownRecipes->InsertEnd(C_RECIPE(knownRecipes_parser[i]));
        }
        for (int i = 0; i < 10; i++) {
            oCItem* itm = new oCItem((zSTRING)"ItPo_Mana_02", 1);
            requiredItems->InsertEnd(*itm);
        }


        //knownRecipes->InsertEnd(C_RECIPE("RECIPE_ItPo_Mana_02"));
        //knownRecipes->InsertEnd(C_RECIPE("RECIPE_ItPo_Health_02"));
        //knownRecipes->InsertEnd(C_RECIPE("RECIPE_ItPo_Perm_Dex"));
        knownRecipesMap->Insert("RECIPE_ItPo_Mana_02", C_RECIPE("RECIPE_ItPo_Mana_02"));
        knownRecipesMap->Insert("RECIPE_ItPo_Health_02", C_RECIPE("RECIPE_ItPo_Health_02"));
        knownRecipesMap->Insert("RECIPE_ItPo_Perm_Dex", C_RECIPE("RECIPE_ItPo_Perm_Dex"));
        
        C_RECIPE* recipe0 = knownRecipes->GetSafe(0);
        oCItem* it00 = new oCItem(recipe0->req_itm[0], 1);
        oCItem* it10 = new oCItem(recipe0->req_itm[1], 1);
        oCItem* it20 = new oCItem(recipe0->req_itm[2], 1);
        oCItem* it30 = new oCItem(recipe0->req_itm[3], 1);
        oCItem* it40 = new oCItem(recipe0->req_itm[4], 1);

        C_RECIPE* recipe1 = knownRecipes->GetSafe(1);
        oCItem* it01 = new oCItem(recipe1->req_itm[0], 1);
        oCItem* it11 = new oCItem(recipe1->req_itm[1], 1);
        oCItem* it21 = new oCItem(recipe1->req_itm[2], 1);
        oCItem* it31 = new oCItem(recipe1->req_itm[3], 1);
        oCItem* it41 = new oCItem(recipe1->req_itm[4], 1);

        C_RECIPE* recipe = knownRecipes->GetSafe(2);
        oCItem* it0 = new oCItem(recipe->req_itm[0], 1);
        oCItem* it1 = new oCItem(recipe->req_itm[1], 1);
        oCItem* it2 = new oCItem(recipe->req_itm[2], 1);
        oCItem* it3 = new oCItem(recipe->req_itm[3], 1);
        oCItem* it4 = new oCItem(recipe->req_itm[4], 1);

        

        /*int func_Render_AddItem = parser->GetIndex("Render_AddItem");
        int func_Render_OpenView = parser->GetIndex("Render_OpenView");
        int itInd_Mana_02 = parser->GetIndex("ItPo_Mana_02");
        int renderItem;
        renderItem = *(int*)parser->CallFunc(func_Render_AddItem, itInd_Mana_02, 0, 0, 4000, 4000);
        parser->CallFunc(func_Render_OpenView, renderItem);*/


        Array<zSTRING> potionCategories;
        potionCategories.Insert("Uzdrawiaj¹ce");
        potionCategories.Insert("Mana");
        potionCategories.Insert("Trwa³e");
        potionCategories.Insert("Czasowe");
        potionCategories.Insert("Inne");



        //for (int i = 0; i < alchemyLevels.GetNum(); i++) {
        //    Menu_Button* lvlTab = new Menu_Button(navBar_x, navBar_y + (btnSizeY + GAPY) * i, alchemyLevels[i]);
        //    lvlTab->name = "Alchemy_Tab_" + (zSTRING)(i + 1);
        //    childsArray.Insert(lvlTab);
        //    //view->InsertItem(lvlTab->view);
        //}

        //Displaying a miniature - remove it from here

        //oCWorld* gameworld = Gothic::Game::Session->GetGameWorld();
        //zCVob* miniature = new zCVob();
        //oCItem* item = nullptr;
        //miniature->dontWriteIntoArchive = True;
        //Gothic::Game::Session->GetGameWorld()->AddVob(miniature);	
        ////miniature->Release();
        //zVEC3 playerPos = player->GetPositionWorld();
        //zVEC3 newMiniaturePos = playerPos + zVEC3(0.0f, 150.0f, 0.0f);
        //miniature->SetPositionWorld(newMiniaturePos);
        ////auto pot = parser->GetSymbol("ItPo_Mana_01");
        ////auto pot = zCObject::CreateNewInstance("ItPo_Mana_01");
        ////auto pot = player->IsInInv("ItPo_Mana_01", 1);
        //oCItem* pot = new oCItem((zSTRING)"ItPo_Mana_01", 1);
        //    //class oCItem* __thiscall oCNpcInventory::IsIn(class zSTRING const&, int)
        //    //class oCItem * __thiscall oCObjectFactory::CreateItem(int)
        //    //static class zCObject * __cdecl zCObject::CreateNewInstance(class zSTRING const &)
        //    //class zCObject * __thiscall zCClassDef::SearchHashTable(class zSTRING const &)
        //pot->CreateVisual();
        //zCVisual* visual = pot->GetVisual();
        //miniature->SetVisual(visual);
        //miniature->GetScreenBBox2D().Draw();
        //
        ////Get subscale
        //zTBBox3D bbox = visual->GetBBox3D();						//Object's 'volume' - rectangle encompassing it
        //float length = (bbox.maxs - bbox.mins).Length();			//Diagonal length
        //float vobSizeMult = SafeDiv(1.0f, length) * 50.0f;		    //We divide the vob by its length and then multiply by bestSize
        //zMAT4& trafo = miniature->GetNewTrafoObjToWorld();			//zMAT4 matrix, taken by reference for easier modification
        ////Reset and rescale
        //trafo.MakeIdentity();										//Reset the matrix (?) - why doesn't the object get distorted then?
        //trafo.PreScale(vobSizeMult);								//Apply the scale along all three axes


        CursorStatic::Show();

    }

    void CraftingView::Close() {
        isOpen = false;
        /*if (renderItem_World) {
            renderItem_World->Release();
            renderItem_World = nullptr;
        }*/
        screen->RemoveItem(view);
        for (Menu_Button* btn : CraftingView::childsArray) {
            view->RemoveItem(btn->view);
            delete btn; btn = nullptr;
        }
        knownRecipes_Instances->Clear();
        knownRecipes->Clear();
        CraftingView::childsArray.Clear();
        CursorStatic::Hide();
    }

    void CraftingView::CallFuncForEachChild(zCView* child, void(zCView::* funcPtr)()) {
        if (child == nullptr) return;                      //Call this function until the object is null
        zCView* nextChild = child->next;                   //This points to the next object in the 'childs' list (needs to be here, as for a strange reason child->next changes after the function is called on the child)
        (child->*funcPtr)();                               //Call the desired function
        CallFuncForEachChild(nextChild, funcPtr);          //Call the same function on the next object
    }
}