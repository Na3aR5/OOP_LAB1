#include <Window.h>
#include <Editor.h>

#include <memory>
#include <thread>

int WINAPI wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ PWSTR pCmdLine,
	_In_ int nCmdShow)
{
	lab::EventSystem eventSystem{};

	lab::WindowCreateInfo windowCreateInfo = {};
	windowCreateInfo.hInstance		 = hInstance;
	windowCreateInfo.nCmdShow		 = nCmdShow;
	windowCreateInfo.pos			 = { 200, 100 };
	windowCreateInfo.size		     = { 720, 480 };
	windowCreateInfo.windowClassName = L"MainWindowClassName";
	windowCreateInfo.windowTitle	 = L"Lab2";
	windowCreateInfo.eventSystem	 = &eventSystem;
	windowCreateInfo.intMenu		 = lab::Editor::GetMainMenuAsIntResource();
	windowCreateInfo.clearColor		 = RGB(255, 255, 255);

	std::unique_ptr<lab::Window> mainWindow = nullptr;
	{
		lab::WindowError windowCreateError;
		mainWindow = std::make_unique<lab::Window>(windowCreateInfo, windowCreateError);
		if (windowCreateError.hasError) {
			return -1;
		}
	}
	std::unique_ptr<lab::Editor> editor = std::make_unique<lab::Editor>(mainWindow.get(), &eventSystem);

	while (mainWindow->IsStillActive()) {
		mainWindow->PollEvents();
		eventSystem.Dispatch();

		mainWindow->Clear();
		editor->Render();
		mainWindow->Present();

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	return 0;
}