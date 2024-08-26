// Supported with union (c) 2020 Union team
// Union SOURCE file
#include <cmath> //for sin, cos
namespace GOTHIC_ENGINE {
    zCOLOR col_Orange = zCOLOR(255, 100, 0);            //Ingredient name on label
    zCOLOR col_RedDark = zCOLOR(167, 11, 11);           //If not enough ingredients
    zCOLOR col_Green = zCOLOR(22, 114, 22);             //If enough ingredients
    zCOLOR col_BrownDark = zCOLOR(90, 51, 39);          //Item Name
    zCOLOR col_BrownLight = zCOLOR(120, 71, 50);        //Item Stats
    zCOLOR col_Transparent = zCOLOR(255, 255, 255);     //Transparent

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
    Menu_Button::Menu_Button(zCView* _parentView, int _sizeX, int _sizeY, zSTRING backTex) {
        parentView = _parentView;
        view->InsertBack(backTex);
        view->SetSize(_sizeX, _sizeY);
    }
    Menu_Button::Menu_Button(int posX, int posY, zSTRING text = "", zCOLOR textColor = zCOLOR(255, 255, 255)) : Menu_Button::Menu_Button() {
        view->SetPos(posX, posY);
        view->CreateText(defaultTextOffsetX, defaultTextOffsetY, text, 50, textColor, 0, 1);
    }
    Menu_Button::Menu_Button(int posX, int posY, int sizeX, int sizeY, zSTRING text = "", zCOLOR textColor = zCOLOR(255, 255, 255)) : Menu_Button::Menu_Button(posX, posY, text, textColor) {
        view->SetSize(sizeX, sizeY);
    }

    void Menu_Button::Render() {
        if (parentView == nullptr) return;
        parentView->InsertItem(view);
        view->Blit();
        parentView->RemoveItem(view);
    }
    void Menu_Button::SetPos(int x, int y) {
        posX = x;
        posY = y;
    }
    
    CraftingScreen::CraftingScreen() {
        renderItem_World = zfactory->CreateWorld();
        #if ENGINE == Engine_G2A
            renderItem_World->m_bIsInventoryWorld = TRUE;
        #endif

        view = new zCView(0, 0, 8192, 8192);
        txtV_ItemStats = new Menu_TextView(view);
        btn_RecItems = new Menu_Button(view, itmSizeX / boxSizeXDivide, itmSizeY / boxSizeYDivide, "AlchemyScreen_Item_Bg.tga");
        btn_Make = new Menu_Button(view, makeSizeX, makeSizeY, "AlchemyScreen_Make.tga");
        btn_Quit = new Menu_Button(view, makeSizeX, makeSizeY, "AlchemyScreen_Quit.tga");
        btn_Tabs = new Menu_Button(view, tabSizeX, tabSizeY, "AlchemyScreen_Tab_Dark.tga");
        btn_Ingr = new Menu_Button(view, ingrSizeX * bgSizeMult, ingrSizeY * bgSizeMult, "AlchemyScreen_Ingr_Bg.tga");
    }


    void CraftingScreen::RenderButton(Menu_Button* btn) {
        view->InsertItem(btn->view);
        btn->view->Blit();
        view->RemoveItem(btn->view);
    }
    void CraftingScreen::RenderButton(int posX, int posY, int sizeX, int sizeY, zSTRING text = "", zCOLOR textColor = zCOLOR(255, 255, 255)) {
        Menu_Button* btn = new Menu_Button(posX, posY, sizeX, sizeY, text, textColor);

        RenderButton(btn);

        delete btn; btn = nullptr;
    }
    
    void CraftingScreen::RenderText(zSTRING text, int posX, int posY, zCOLOR color = zCOLOR(255, 255, 255, 255), zSTRING font = "Font_Default.tga", int fontHeight = 0) {
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

    void CraftingScreen::RenderItem(oCItem* item, int posX, int posY, int sizeX, int sizeY)
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
    void CraftingScreen::RenderItem(zSTRING itemName, int posX, int posY, int sizeX, int sizeY)
    {
        if (oCItem* item = new oCItem(itemName, 1)) {
            RenderItem(item, posX, posY, sizeX, sizeY);
            item->Release(); //This is for new oCItem
        }
    }
    void CraftingScreen::RenderItem(int itemInstance, int posX, int posY, int sizeX, int sizeY)
    {
        if (oCItem* item = new oCItem(itemInstance, 1)) {
            RenderItem(item, posX, posY, sizeX, sizeY);
            item->Release(); //This is for new oCItem
        }
    }

    void CraftingScreen::Update_Alchemy() {
        //Render Quit and Make buttons
        btn_Make->SetPos(4550, 880);
        btn_Make->view->SetSize(300, 500);
        if (IsCursorHovering(btn_Make)) { btn_Make->view->SetColor(zCOLOR(225, 255, 225)); }
        else                            { btn_Make->view->SetColor(zCOLOR(200, 240, 200)); }
        //Below is working
        /*if (IsClicked(btn_Make) && selectedRecipeInd >= 0) {
            C_RECIPE* selRec = knownRecipes->GetSafe(selectedRecipeInd);
            oCItem* selItm = new oCItem(selRec->create_item, 1);
            player->inventory2.Insert(selItm);
            selItm->Release();
        }*/
        btn_Make->Render();

        btn_Quit->SetPos(3300, 880);
        btn_Quit->view->SetSize(300, 500);
        if (IsCursorHovering(btn_Quit)) { btn_Quit->view->SetColor(zCOLOR(255, 225, 225)); }
        else                            { btn_Quit->view->SetColor(zCOLOR(240, 200, 200)); }
        if (IsClicked(btn_Quit)) { 
            CraftingScreen::Close(); 
            return; 
        }
        btn_Quit->Render();

        //Render tabs
        for (int i = 0; i < NUM_OF_TABS; i++) {
            btn_Tabs->SetPos(tabPosX + i * (tabSizeX - 10), tabPosY);
            btn_Tabs->view->InsertBack("AlchemyScreen_Tab_" + (zSTRING)i);
            if (IsClicked(btn_Tabs)) {
                bool* b = selectedTabInd.GetSafe(i);
                *b = !*b;
            }

            if (*selectedTabInd.GetSafe(i) == true) {
                btn_Tabs->view->SetColor(col_Transparent);
            }
            else {
                if (IsCursorHovering(btn_Tabs)) {
                    btn_Tabs->view->SetColor(zCOLOR(250, 215, 150));
                }
                else {
                    btn_Tabs->view->SetColor(zCOLOR(200, 165, 110));
                }
            }
            btn_Tabs->Render();
        }

        //Render item for each known recipe (left side of the screen)
        for (int i = 0; i < knownRecipes->GetNum(); i++) {
            btn_RecItems->SetPos(itmBasePosX + (itmSizeX * boxPosXOffsetMult) + gapX * i, itmBasePosY + (itmSizeY * boxPosYOffsetMult));
            if (IsCursorHovering(btn_RecItems) && zinput->GetMouseButtonPressedLeft() == 1) {
                selectedRecipeInd = i;
            }
            btn_RecItems->Render();

            C_RECIPE* rec = knownRecipes->GetSafe(i);
            oCItem* itm = new oCItem(rec->create_item, 1);

            RenderItem(itm, itmBasePosX + gapX * i, itmBasePosY, itmSizeX, itmSizeY); //Here RenderItem increases itm->RefCtr by 2
            itm->Release();

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
            RenderItem(selItm, 6300, 1700, itmSizeX * 1.7, itmSizeY * 1.7); //Here RenderItem does not increase itm->RefCtr

            //Ingredients required for the recipe
            bool renderIngrLabel = false;
            int renderIngrLabelX = 0;
            int renderIngrLabelY = 0;
            zSTRING renderIngrLabelName = "";

            int i = 0;
            notEnoughIngredients = false;
            while (selRec->req_itm[i] > 0 && i < 9) { //TODO: i < 9 (change to < than array)
                int instance = selRec->req_itm[i];

                //Values for displaying required ingredients in a circle
                int ingrPosX = itmCenterX + radius * sin(3.14 + 6.28f / 9.0f * i);
                int ingrPosY = itmCenterY + radius * 1.78f * cos(3.14 + 6.28f / 9.0f * i);
                int bgPosOffsetX = ingrSizeX * (1 - bgSizeMult) / 2.0f; 
                int bgPosOffsetY = ingrSizeY * (1 - bgSizeMult) / 2.0f;

                //Check required and possesed items
                player->inventory2.UnpackAllItems();
                //auto it = player->IsInInv(ingr->objectName, 1);
                int ingrAmountInInv = player->inventory2.GetAmount(instance);
                int ingrAmountRequired = selRec->req_qty[i];
                zCOLOR col1 = ingrAmountInInv < ingrAmountRequired ? zCOLOR(167, 11, 11) : zCOLOR(22, 114, 22);
                zCOLOR col2 = zCOLOR(120, 71, 50); //(120, 71, 50) //179, 139, 92
                notEnoughIngredients = ingrAmountRequired > ingrAmountInInv ? true : false;

                //Render button
                btn_Ingr->SetPos(ingrPosX + bgPosOffsetX, ingrPosY + bgPosOffsetY);
                btn_Ingr->view->CreateText(1500, 5000 + 800, ingrAmountInInv, 0, col1, 0, 1);   //Left-right: 1500, 5000; UppperRightCircle: 5200, 0; LeftUp-BottomRight: 1000, 0
                btn_Ingr->view->CreateText(5500, 5000 + 800, ingrAmountRequired, 0, col2, 0, 1); //Left-right: 5500, 5000; UppperRightCircle: 7000, 2000; LeftUp-BottomRight: 7000, 4000
                //ingrBtn->view->CreateText(6100, 1000, "/ ", 0, zCOLOR(179, 139, 92), 0, 1);
                btn_Ingr->Render();

                oCItem* ingr = new oCItem(instance, 1);
                RenderItem(ingr, ingrPosX, ingrPosY, ingrSizeX, ingrSizeY);
                
                //If cursor is hovering over an ingredient, update the vars to render ingr name later below
                if (IsCursorHovering(btn_Ingr)) {
                    renderIngrLabel = true;
                    //Label always displayed in the middle
                    renderIngrLabelX = ingrPosX + bgPosOffsetY + ingrSizeX / 2 - 1250 / 2;
                    renderIngrLabelY = ingrPosY + bgPosOffsetY - ingrSizeY / 2;
                    renderIngrLabelName = ingr->description;
                }
                i++;
                ingr->Release();
            }
            if (IsClicked(btn_Make) && notEnoughIngredients == false) {
                player->inventory2.Insert(selItm);
                int j = 0;
                while (selRec->req_itm[j] > 0 && j < 9) { //TODO: i < 9 (change to < than array)
                    player->inventory2.Remove(selRec->req_itm[j], selRec->req_qty[j]);
                    j++;
                }
                //To remove items:
                //player->inventory2.Remove(selRec->create_item, 3);
                //player->inventory2.RemoveByPtr(selItm, 3);
            }
            selItm->Release();
            //Display ingredient name when hovering
            if (renderIngrLabel) {
                Menu_Button::defaultBackTex = "AlchemyScreen_Ingr_NameLabel.tga";
                Menu_Button::defaultTextOffsetX = 500;
                Menu_Button::defaultTextOffsetY = 2500;
                RenderButton(renderIngrLabelX, renderIngrLabelY, 1250, 480, renderIngrLabelName, zCOLOR(255, 100, 0));
            }
            
        }
    }


    

    void CraftingScreen::Update() {
        if (craftingScreen == nullptr) return;
        /*if (!ogame || ogame->IsOnPause()) {
            return;
        }*/

        screen->InsertItem(craftingScreen->view);
        craftingScreen->view->Blit();

        craftingScreen->Update_Alchemy();   //TO DO: Add sth like if CraftingType == Alchemy, else if CraftingType == Smithing, etc.

        //This line needs to be here, as closing the crafting screen after clicking on quit button happens in Update_Alchemy()
        //And if that happens cragting screen is already nullptr and is already removed from screen.
        if (craftingScreen == nullptr) return; 
        screen->RemoveItem(craftingScreen->view);
    }

    void CraftingScreen::Open_Alchemy() {
        if (craftingScreen != nullptr) return; //If somehow it already exists, don't make a new one
        craftingScreen = new CraftingScreen();
        craftingScreen->view->InsertBack("AlchemyScreen_Background.tga");
        craftingScreen->NUM_OF_TABS = 6;
        for (int i = 0; i < craftingScreen->NUM_OF_TABS; i++) {
            craftingScreen->selectedTabInd.Insert(false);
        }
        
        screen->InsertItem(craftingScreen->view);
        craftingScreen->view->Blit();

        zCPar_Symbol* knownRecipes_sym = parser->GetSymbol("KnownRecipes");
        int knownRecipes_max = parser->GetSymbol("KnownRecipes_Max")->single_intdata; 
        int* knownRecipes_parser = (int*)knownRecipes_sym->adr;                         //Address of the array
        for (int i = 0; i < knownRecipes_sym->ele; i++) {
            if (knownRecipes_parser[i] <= 0) continue;
            knownRecipes_Instances->InsertEnd(knownRecipes_parser[i]);
            knownRecipes->InsertEnd(C_RECIPE(knownRecipes_parser[i]));
        }

        CursorStatic::Show();

    }

    void CraftingScreen::Close() {
        if (craftingScreen == nullptr) return; //If somehow it already doesn't exist, don't delete it again
        screen->RemoveItem(craftingScreen->view);
        knownRecipes_Instances->Clear();
        knownRecipes->Clear();
        CursorStatic::Hide();
        delete craftingScreen; craftingScreen = nullptr;
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
}