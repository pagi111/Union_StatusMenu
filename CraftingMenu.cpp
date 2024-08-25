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
    zCView* textBtnView = new zCView();
    //Menu_Button* testBtn = new Menu_Button();
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
    Menu_Button::Menu_Button(int posX, int posY, zSTRING text = "", zCOLOR textColor = zCOLOR(255, 255, 255)) : Menu_Button::Menu_Button() {
        view->SetPos(posX, posY);
        view->CreateText(defaultTextOffsetX, defaultTextOffsetY, text, 50, textColor, 0, 1);
    }
    Menu_Button::Menu_Button(int posX, int posY, int sizeX, int sizeY, zSTRING text = "", zCOLOR textColor = zCOLOR(255, 255, 255)) : Menu_Button::Menu_Button(posX, posY, text, textColor) {
        view->SetSize(sizeX, sizeY);
    }

    CraftingScreen::CraftingScreen() {
        renderItem_World = zfactory->CreateWorld();
        #if ENGINE == Engine_G2A
            renderItem_World->m_bIsInventoryWorld = TRUE;
        #endif

        view = new zCView(0, 0, 8192, 8192);
        txtV_ItemStats = new Menu_TextView(view);
    }


    void CraftingScreen::RenderButton(Menu_Button* btn) {
        Menu_Button::defaultTextOffsetX = 2500;

        btn->view->SetPos(btn->posX, btn->posY);
        btn->view->SetSize(btn->sizeX, btn->sizeY);

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
        //Render tabs
        for (int i = 0; i < 5; i++) {
            //Working
            //Menu_Button::defaultBackTex = "AlchemyScreen_Tab_Dark.tga";
            //Menu_Button* btn = new Menu_Button(tabPosX + i * (tabSizeX - 10), tabPosY, tabSizeX, tabSizeY);

            //if (IsCursorHovering(btn)) {
            //    //btn->view->InsertBack("AlchemyScreen_Tab_Light.tga");
            //    btn->view->SetColor(col_Transparent);
            //}
            //else {
            //    //btn->view->InsertBack("AlchemyScreen_Tab_Dark.tga");
            //    btn->view->SetColor(zCOLOR(200, 165, 110));
            //}

            //CraftingView::RenderButton(btn);
            //delete btn;

            //Test View - working
            textBtnView->InsertBack("AlchemyScreen_Tab_Dark.tga");
            textBtnView->SetPos(tabPosX + i * (tabSizeX - 10), tabPosY);
            textBtnView->SetSize(tabSizeX, tabSizeY);
            if (IsCursorHovering(textBtnView)) {
                //btn->view->InsertBack("AlchemyScreen_Tab_Light.tga");
                textBtnView->SetColor(col_Transparent);
            }
            else {
                //btn->view->InsertBack("AlchemyScreen_Tab_Dark.tga");
                textBtnView->SetColor(zCOLOR(200, 165, 110));
            }
            view->InsertItem(textBtnView);
            textBtnView->Blit();
            view->RemoveItem(textBtnView);

            //Test Btn
            //testBtn->view->InsertBack("AlchemyScreen_Tab_Dark.tga");
            //testBtn->view->SetPos(tabPosX + i * (tabSizeX - 10), tabPosY);
            //testBtn->view->SetSize(tabSizeX, tabSizeY);
            //if (IsCursorHovering(testBtn)) {
            //    //btn->view->InsertBack("AlchemyScreen_Tab_Light.tga");
            //    testBtn->view->SetColor(col_Transparent);
            //}
            //else {
            //    //btn->view->InsertBack("AlchemyScreen_Tab_Dark.tga");
            //    testBtn->view->SetColor(zCOLOR(200, 165, 110));
            //}
            //RenderButton(testBtn);

        }

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

            RenderButton(btn);
            RenderItem(itm, itmBasePosX + gapX * i, itmBasePosY, itmSizeX, itmSizeY); //Here RenderItem increases itm->RefCtr by 2
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
            RenderItem(selItm, 6300, 1700, itmSizeX * 1.7, itmSizeY * 1.7); //Here RenderItem does not increase itm->RefCtr
            selItm->Release();

            //Ingredients required for the recipe
            bool renderIngrLabel = false;
            int renderIngrLabelX = 0;
            int renderIngrLabelY = 0;
            zSTRING renderIngrLabelName = "";

            int i = 0;
            while (selRec->req_itm[i] > 0 && i < 9) {
                int instance = selRec->req_itm[i];
                //oCItem* item = new oCItem(requiredItems->GetSafe(i)->instanz, 1); //Same as above
                //RenderButton(ingrBasePosX + (itmSizeX * 0.30) + gapX * i, ingrBasePosY + (itmSizeY * 0.15), itmSizeX / 2.5, itmSizeY / 1.25);
                //RenderItem(instance, ingrBasePosX + gapX * i, ingrBasePosY, itmSizeX*0.6, itmSizeY*0.6);
                
                //Values for displaying required ingredients in a circle
                int ingrPosX = itmCenterX + radius * sin(3.14 + 6.28f / 9.0f * i);
                int ingrPosY = itmCenterY + radius * 1.78f * cos(3.14 + 6.28f / 9.0f * i);
                int ingrSizeX = itmSizeX * 0.6;
                int ingrSizeY = itmSizeY * 0.6;
                float bgSizeMult = 0.90f;
                int bgPosOffsetX = ingrSizeX * (1 - bgSizeMult) / 2.0f; //If we make bg smaller, then we need to add to the pos the difference between ingr size and bg size and then divide it by 2
                int bgPosOffsetY = ingrSizeY * (1 - bgSizeMult) / 2.0f;

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
                    //Option 1 - name label displayed to the left as much as needed, if it would be off screen
                    //renderIngrLabelX = ingrPosX + bgPosOffsetX + ingrSizeX / 2;
                    //int offsetXIfOffScreen = renderIngrLabelX + 1250 - 8192;
                    //renderIngrLabelX = offsetXIfOffScreen > 0 ? renderIngrLabelX - offsetXIfOffScreen : renderIngrLabelX;
                    //Option 2 - name label always displayed in the middle
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


    

    void CraftingScreen::Update() {
        if (craftingScreen == nullptr) return;
        /*if (!ogame || ogame->IsOnPause()) {
            return;
        }*/

        screen->InsertItem(craftingScreen->view);
        craftingScreen->view->Blit();

        craftingScreen->Update_Alchemy();   //TO DO: Add sth like if CraftingType == Alchemy, else if CraftingType == Smithing, etc.

        screen->RemoveItem(craftingScreen->view);
    }

    void CraftingScreen::Open_Alchemy() {
        craftingScreen = new CraftingScreen();
        craftingScreen->view->InsertBack("AlchemyScreen_Background_1.tga");
        

        screen->InsertItem(craftingScreen->view);
        craftingScreen->view->Blit();

        zCPar_Symbol* knownRecipes_sym = parser->GetSymbol("KnownRecipes");
        int knownRecipes_max = parser->GetSymbol("KnownRecipes_Max")->single_intdata; 
        int* knownRecipes_parser = (int*)knownRecipes_sym->adr;                         //Address of the array
        for (int i = 0; i < knownRecipes_sym->ele; i++) {
            knownRecipes_Instances->InsertEnd(knownRecipes_parser[i]);
            knownRecipes->InsertEnd(C_RECIPE(knownRecipes_parser[i]));
        }

        CursorStatic::Show();

    }

    void CraftingScreen::Close() {
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