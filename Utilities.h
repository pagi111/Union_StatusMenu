// Supported with union (c) 2020 Union team
// Union HEADER file

namespace GOTHIC_ENGINE {
    void TrimStringFront(zSTRING& s) {
        if (s.StartWith(" ") || s.StartWith("\t")) {
            s.Cut(0, 1);            //Cut 1 char from the beginning
            TrimStringFront(s);
        }
    }
    void TrimStringEnd(zSTRING& s) {
        if (s.EndWith(" ") || s.EndWith("\t")) {
            s.Cut(s.Length() - 1, 1);   //Cut 1 char from the end
            TrimStringEnd(s);
        }
    }
    void TrimString(zSTRING& s) {
        TrimStringFront(s);
        TrimStringEnd(s);
    }

    void SplitString(zSTRING s, zSTRING separator, Array<zSTRING>& arr) {
        int length = s.Length();
        Common::uint32 ind = s.Search(separator, 0, length);
        if (ind == -1) {
            arr.InsertEnd(s);
            return;
        }
        zSTRING s2 = zSTRING(s);
        zSTRING first = s2.Cut(ind - 1, length);
        zSTRING second = s.Cut(0, ind);
        arr.InsertEnd(first);
        SplitString(second, separator, arr);
    }
}