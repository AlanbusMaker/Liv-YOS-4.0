#include <windows.h>
#include <commctrl.h>
#include <string.h>

#define ID_PRINT_BUTTON     101
#define ID_DOC_INPUT        102
#define ID_DOC_LIST         103
#define IDT_CLOSE_PRINTING  2001

const char g_szClassName[] = "ImpresserProMain";
const char g_szPrintWnd[]  = "PrintingWindow";

HWND hInputField, hDocListWnd, hListBox;
HINSTANCE g_hInstance;

// ---------------- JANELA DE IMPRESSÃO ----------------
LRESULT CALLBACK PrintingWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE: {
        hListBox = CreateWindowEx(0, WC_LISTBOX, NULL,
            WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_NOTIFY,
            10, 10, 260, 100,
            hwnd, (HMENU)ID_DOC_LIST, g_hInstance, NULL);

        SetTimer(hwnd, IDT_CLOSE_PRINTING, 5000, NULL);
        break;
    }
    case WM_TIMER:
        if (wParam == IDT_CLOSE_PRINTING) {
            KillTimer(hwnd, IDT_CLOSE_PRINTING);
            DestroyWindow(hwnd);
            hDocListWnd = NULL;
        }
        break;
    case WM_CLOSE:
        DestroyWindow(hwnd);
        hDocListWnd = NULL;
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

void ShowPrintingWindow() {
    if (hDocListWnd != NULL) {
        return; // já tá aberta
    }

    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = PrintingWndProc;
    wc.hInstance = g_hInstance;
    wc.lpszClassName = g_szPrintWnd;
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);

    RegisterClass(&wc);

    int screenY = GetSystemMetrics(SM_CYSCREEN);

    hDocListWnd = CreateWindowEx(
        WS_EX_TOPMOST, g_szPrintWnd, NULL,
        WS_POPUP | WS_VISIBLE,
        10, screenY - 160, 280, 130,
        NULL, NULL, g_hInstance, NULL);

    ShowWindow(hDocListWnd, SW_SHOW);
    UpdateWindow(hDocListWnd);
}

// ---------------- JANELA PRINCIPAL ----------------
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE: {
        CreateWindow("STATIC", "Impresser Pro 2.5",
            WS_CHILD | WS_VISIBLE,
            20, 20, 200, 20,
            hwnd, NULL, g_hInstance, NULL);

        hInputField = CreateWindow("EDIT", "",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
            20, 50, 240, 24,
            hwnd, (HMENU)ID_DOC_INPUT, g_hInstance, NULL);

        CreateWindow("BUTTON", "Imprimir",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            270, 50, 90, 25,
            hwnd, (HMENU)ID_PRINT_BUTTON, g_hInstance, NULL);
        break;
    }
    case WM_COMMAND:
        if (LOWORD(wParam) == ID_PRINT_BUTTON) {
            char docName[256];
            GetWindowText(hInputField, docName, sizeof(docName));

            if (strlen(docName) == 0) {
                MessageBox(hwnd, "Digite o nome do documento!", "Erro", MB_ICONWARNING);
                break;
            }

            ShowPrintingWindow();

            // Adiciona à lista da janela de impressão
            if (hListBox) {
                SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)docName);
            }

            // Limpa input
            SetWindowText(hInputField, "");
        }
        break;
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

// ---------------- MAIN ----------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow) {

    g_hInstance = hInstance;

    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = g_szClassName;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(0, g_szClassName, "Impresser Pro 2.5",
        WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 160,
        NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG Msg;
    while (GetMessage(&Msg, NULL, 0, 0)) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    return Msg.wParam;
}
