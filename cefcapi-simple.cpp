// cefcapi-simple.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "framework.h"
#include "cefcapi-simple.h"

#define MAX_LOADSTRING 100

// グローバル変数:
HINSTANCE hInst;                                // 現在のインターフェイス
WCHAR szTitle[MAX_LOADSTRING];                  // タイトル バーのテキスト
WCHAR szWindowClass[MAX_LOADSTRING];            // メイン ウィンドウ クラス名

// Globals
cef_life_span_handler_t g_life_span_handler = {};

// このコード モジュールに含まれる関数の宣言を転送します:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: ここにコードを挿入してください。
    // Main args
    cef_main_args_t main_args = {};
    main_args.instance = hInstance;

    // Cef app
    cef_app_t app = {};
    initialize_cef_app(&app);

//    printf("cef_execute_process, argc=%d\n", argc);
    int code = cef_execute_process(&main_args, &app, NULL);
    if (code >= 0) {
        _exit(code);
    }

    // Application settings. It is mandatory to set the
    // "size" member.
    cef_settings_t settings = {};
    settings.size = sizeof(cef_settings_t);
    settings.log_severity = LOGSEVERITY_WARNING; // Show only warnings/errors
    settings.no_sandbox = 1;

    // Initialize CEF
    printf("cef_initialize\n");
    cef_initialize(&main_args, &settings, &app, NULL);

    // Window info
    cef_window_info_t window_info = {};
    window_info.style = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN \
        | WS_CLIPSIBLINGS | WS_VISIBLE;
    window_info.parent_window = NULL;
    window_info.x = CW_USEDEFAULT;
    window_info.y = CW_USEDEFAULT;
    window_info.width = CW_USEDEFAULT;
    window_info.height = CW_USEDEFAULT;

    // Window info - window title
    char window_name[] = "cefcapi example";
    cef_string_t cef_window_name = {};
    cef_string_utf8_to_utf16(window_name, strlen(window_name),
        &cef_window_name);
    window_info.window_name = cef_window_name;

    // Initial url
    char url[] = "https://www.google.com/ncr";
    cef_string_t cef_url = {};
    cef_string_utf8_to_utf16(url, strlen(url), &cef_url);

    // Browser settings. It is mandatory to set the
    // "size" member.
    cef_browser_settings_t browser_settings = {};
    browser_settings.size = sizeof(cef_browser_settings_t);

    // Client handlers
    cef_client_t client = {};
    initialize_cef_client(&client);
    initialize_cef_life_span_handler(&g_life_span_handler);

    // Create browser asynchronously. There is also a
    // synchronous version of this function available.
    printf("cef_browser_host_create_browser\n");
    cef_browser_host_create_browser(&window_info, &client, &cef_url,
        &browser_settings, NULL, NULL);

    // Message loop. There is also cef_do_message_loop_work()
    // that allow for integrating with existing message loops.
    // On Windows for best performance you should set
    // cef_settings_t.multi_threaded_message_loop to true.
    // Note however that when you do that CEF UI thread is no
    // more application main thread and using CEF API is more
    // difficult and require using functions like cef_post_task
    // for running tasks on CEF UI thread.
    printf("cef_run_message_loop\n");
    cef_run_message_loop();

    // Shutdown CEF
    printf("cef_shutdown\n");
    cef_shutdown();

    exit(0);

    //// グローバル文字列を初期化する
    //LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    //LoadStringW(hInstance, IDC_CEFCAPISIMPLE, szWindowClass, MAX_LOADSTRING);
    //MyRegisterClass(hInstance);

    //// アプリケーション初期化の実行:
    //if (!InitInstance (hInstance, nCmdShow))
    //{
    //    return FALSE;
    //}

    //HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CEFCAPISIMPLE));

    //MSG msg;

    //// メイン メッセージ ループ:
    //while (GetMessage(&msg, nullptr, 0, 0))
    //{
    //    if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
    //    {
    //        TranslateMessage(&msg);
    //        DispatchMessage(&msg);
    //    }
    //}

    //return (int) msg.wParam;
}



//
//  関数: MyRegisterClass()
//
//  目的: ウィンドウ クラスを登録します。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CEFCAPISIMPLE));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_CEFCAPISIMPLE);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   関数: InitInstance(HINSTANCE, int)
//
//   目的: インスタンス ハンドルを保存して、メイン ウィンドウを作成します
//
//   コメント:
//
//        この関数で、グローバル変数でインスタンス ハンドルを保存し、
//        メイン プログラム ウィンドウを作成および表示します。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // グローバル変数にインスタンス ハンドルを格納する

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  関数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的: メイン ウィンドウのメッセージを処理します。
//
//  WM_COMMAND  - アプリケーション メニューの処理
//  WM_PAINT    - メイン ウィンドウを描画する
//  WM_DESTROY  - 中止メッセージを表示して戻る
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 選択されたメニューの解析:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: HDC を使用する描画コードをここに追加してください...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// バージョン情報ボックスのメッセージ ハンドラーです。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
