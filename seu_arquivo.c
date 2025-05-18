#include <windows.h>
#include <commctrl.h>
#include <richedit.h>
#include <stdio.h>

#ifndef MSFTEDIT_CLASS
#define MSFTEDIT_CLASS "RICHEDIT50W"
#endif

#define ID_EDIT 1
#define ID_BOLD 101
#define ID_ITALIC 102
#define ID_UNDERLINE 103
#define ID_SAVE 104

HWND hEdit;
HWND hBtnBold, hBtnItalic, hBtnUnderline, hBtnSave;

// Função pra aplicar estilo no texto selecionado
void ToggleCharFormat(HWND hwndEdit, DWORD mask, DWORD effect) {
    CHARFORMAT cf = { sizeof(CHARFORMAT) };
    SendMessage(hwndEdit, EM_GETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);

    if (cf.dwEffects & effect)
        cf.dwEffects &= ~effect;  // tira o estilo
    else
        cf.dwEffects |= effect;   // aplica o estilo

    cf.dwMask = mask;
    SendMessage(hwndEdit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
}

// Salvar texto em arquivo simples UTF-8
void SaveToFile(HWND hwndEdit) {
    int length = GetWindowTextLength(hwndEdit);
    char *buffer = (char*)malloc(length + 1);
    if (!buffer) return;

    GetWindowText(hwndEdit, buffer, length + 1);

    // Salvar no arquivo "livtexti.txt"
    FILE *f = fopen("livtexti.txt", "w");
    if (f) {
        fwrite(buffer, 1, length, f);
        fclose(f);
        MessageBox(hwndEdit, "Arquivo salvo como livtexti.txt", "Salvo!", MB_OK);
    } else {
        MessageBox(hwndEdit, "Erro ao salvar o arquivo", "Erro", MB_ICONERROR);
    }

    free(buffer);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch(msg) {
        case WM_CREATE: {
            LoadLibrary("Msftedit.dll");

            hEdit = CreateWindowEx(0, MSFTEDIT_CLASS, NULL,
                WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL |
                ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
                0, 0, 0, 0,
                hwnd, (HMENU)ID_EDIT, GetModuleHandle(NULL), NULL);

            // Criar botões
            hBtnBold = CreateWindow("BUTTON", "Negrito", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                10, 0, 80, 25, hwnd, (HMENU)ID_BOLD, GetModuleHandle(NULL), NULL);

            hBtnItalic = CreateWindow("BUTTON", "Itálico", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                100, 0, 80, 25, hwnd, (HMENU)ID_ITALIC, GetModuleHandle(NULL), NULL);

            hBtnUnderline = CreateWindow("BUTTON", "Sublinhado", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                190, 0, 100, 25, hwnd, (HMENU)ID_UNDERLINE, GetModuleHandle(NULL), NULL);

            hBtnSave = CreateWindow("BUTTON", "Salvar", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                300, 0, 80, 25, hwnd, (HMENU)ID_SAVE, GetModuleHandle(NULL), NULL);

            break;
        }
        case WM_SIZE: {
            int width = LOWORD(lParam);
            int height = HIWORD(lParam);

            // Altura da barra de botões
            int barHeight = 30;

            // Posicionar editor ocupando tudo menos a barra de botões embaixo
            MoveWindow(hEdit, 0, 0, width, height - barHeight, TRUE);

            // Posicionar barra de botões na parte inferior
            MoveWindow(hBtnBold, 10, height - barHeight + 2, 80, 25, TRUE);
            MoveWindow(hBtnItalic, 100, height - barHeight + 2, 80, 25, TRUE);
            MoveWindow(hBtnUnderline, 190, height - barHeight + 2, 100, 25, TRUE);
            MoveWindow(hBtnSave, 300, height - barHeight + 2, 80, 25, TRUE);

            break;
        }
        case WM_COMMAND: {
            switch(LOWORD(wParam)) {
                case ID_BOLD:
                    ToggleCharFormat(hEdit, CFM_BOLD, CFE_BOLD);
                    break;
                case ID_ITALIC:
                    ToggleCharFormat(hEdit, CFM_ITALIC, CFE_ITALIC);
                    break;
                case ID_UNDERLINE:
                    ToggleCharFormat(hEdit, CFM_UNDERLINE, CFE_UNDERLINE);
                    break;
                case ID_SAVE:
                    SaveToFile(hEdit);
                    break;
            }
            break;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow) {

    WNDCLASS wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "SimpleRichEdit";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_IBEAM);

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(0, wc.lpszClassName, "LivTexti",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        NULL, NULL, hInstance, NULL);

    if (!hwnd) return 0;

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}
