#include <windows.h>
#include <commdlg.h>

#define ID_ABOUT 1
#define ID_OPEN  2

HBITMAP hBitmap = NULL;
BITMAP bitmap;

void LoadAndDisplayBMP(HWND hwnd, const char* path) {
    if (hBitmap) {
        DeleteObject(hBitmap);
        hBitmap = NULL;
    }

    hBitmap = (HBITMAP) LoadImageA(NULL, path, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    if (!hBitmap) {
        MessageBox(hwnd, "Erro ao carregar a imagem BMP.", "Erro", MB_OK | MB_ICONERROR);
        return;
    }

    GetObject(hBitmap, sizeof(bitmap), &bitmap);

    // Redimensiona janela pro tamanho da imagem
    SetWindowPos(hwnd, NULL, 0, 0, bitmap.bmWidth + 16, bitmap.bmHeight + 39, SWP_NOMOVE | SWP_NOZORDER);
    InvalidateRect(hwnd, NULL, TRUE);
}

char* OpenBMPDialog(HWND owner) {
    static char filename[MAX_PATH] = "";

    OPENFILENAME ofn = {0};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = owner;
    ofn.lpstrFilter = "Bitmap Files (*.bmp)\0*.bmp\0Todos os Arquivos (*.*)\0*.*\0";
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
    ofn.lpstrTitle = "Escolha uma imagem BMP";

    if (GetOpenFileName(&ofn)) {
        return filename;
    } else {
        return NULL;
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case ID_ABOUT:
                    MessageBox(hwnd,
                        "Viwert 2.5\nVisualizador oficial de imagens BMP do Liv YOS.\nDesde o Liv YOS 3.0 ao lado de Impresser e LivTexti.",
                        "Sobre o Viwert", MB_OK | MB_ICONINFORMATION);
                    break;

                case ID_OPEN: {
                    char* bmpPath = OpenBMPDialog(hwnd);
                    if (bmpPath) LoadAndDisplayBMP(hwnd, bmpPath);
                    break;
                }
            }
            break;

        case WM_PAINT:
            if (hBitmap) {
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hwnd, &ps);
                HDC hdcMem = CreateCompatibleDC(hdc);
                SelectObject(hdcMem, hBitmap);
                BitBlt(hdc, 0, 0, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, SRCCOPY);
                DeleteDC(hdcMem);
                EndPaint(hwnd, &ps);
            }
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

HMENU CreateAppMenu() {
    HMENU hMenu = CreateMenu();
    HMENU hFile = CreateMenu();
    HMENU hHelp = CreateMenu();

    AppendMenu(hFile, MF_STRING, ID_OPEN, "Abrir...");
    AppendMenu(hHelp, MF_STRING, ID_ABOUT, "Sobre Viwert");

    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFile, "Arquivo");
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hHelp, "Ajuda");

    return hMenu;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const char CLASS_NAME[] = "ViwertWindowClass";

    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0, CLASS_NAME, "Viwert 2.5",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 640, 480,
        NULL, NULL, hInstance, NULL
    );

    if (!hwnd) return 0;

    SetMenu(hwnd, CreateAppMenu());

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Primeira imagem
    char* bmpPath = OpenBMPDialog(hwnd);
    if (bmpPath) LoadAndDisplayBMP(hwnd, bmpPath);

    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (hBitmap) DeleteObject(hBitmap);
    return 0;
}
