// Supported with union (c) 2020 Union team
// Union HEADER file

namespace GOTHIC_ENGINE {
    class CursorStatic {
    public:
        inline static float x, y;
        inline static float deltaX, deltaY, deltaZ;     //deltaZ is only needed for zinput->GetMousePos() but it seems to always be 0
        inline static int sensitivityX, sensitivityY;   //10 should be fine. Less than 5 or more than 15 probably not recommended.
        inline static bool visible;
        inline static zCView view;

        static void Init() {
            sensitivityX = sensitivityY = 10;
            view.SetSize(250, 250);
        }
        static void Show() {
            view.Open();            //Open() here, or screen->InsertItem(&view) and view.Blit() in Update
            visible = true;
        }

        static void Hide() {
            view.Close();           //Close() here, or screen->RemoveItem(&view) in Update
            visible = false;
        }

        //This should be called every frame (or at least every frame if the cursor is needed)
        static void Update(bool debugPrint = false) {
            if (!visible) return;   //Skip the rest if cursor is not currently visible

            //screen->InsertItem(&view);    //This here, or view.Open() in Show()
            //view.Blit();

            zinput->GetMousePos(deltaX, deltaY, deltaZ);
            x += sensitivityX * deltaX;
            if (x < 0) { x = 0; }
            else if (x > 8192) { x = 8192; }
            y += sensitivityY * deltaY * 2;
            if (y < 0) { y = 0; }
            else if (y > 8192) { y = 8192; }
            view.SetPos(x, y);

            if (zinput->GetMouseButtonPressedLeft() == 1) {
                view.InsertBack("CursorHand_Pressed.tga");
            }
            else {
                view.InsertBack("CursorHand_Regular.tga");
            }
            view.Top();     //This makes the cursor always on top of other zCViews (this should work, although if another view is created in Game_Loop after cursor Update, it will probably cover the cursor anyway)

            if (debugPrint) {
                screen->Print(200, 400, "x: " + (zSTRING)x);
                screen->Print(200, 600, "y: " + (zSTRING)y);
            }

            //screen->RemoveItem(&view);    //This here, or view.Close() in Hide()

        }
    };
}