#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl2.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <vector>
#include <string>
#include <map>
#include <algorithm>

// Data structures
struct QueueEntry {
    std::string title;
    std::string requester;
    std::string creator;
    bool expanded;
    
    QueueEntry(const std::string& t = "", const std::string& r = "", const std::string& c = "")
        : title(t), requester(r), creator(c), expanded(false) {}
};

struct BannedUser {
    std::string name;
    bool isRequester; // true = requester, false = creator
};

class HwGDReqsApp {
public:
    HwGDReqsApp() {
        // Initialize queue with sample data
        queueEntries.push_back(QueueEntry("Bloodbath", "riot", "acoolgamer"));
        queueEntries.push_back(QueueEntry("Tidal Wave", "onilink", "anotherperson"));
        
        // Fill remaining slots with empty entries
        for (int i = 0; i < 4; i++) {
            queueEntries.push_back(QueueEntry());
        }
        
        // Sample banned users
        bannedUsers.push_back({"Requester_Alpha", true});
        bannedUsers.push_back({"Requester_Gamma", true});
        bannedUsers.push_back({"Requester_Omega", true});
        bannedUsers.push_back({"Creator_42", false});
        bannedUsers.push_back({"Creator_Blitz", false});
        bannedUsers.push_back({"Creator_Nyx", false});
        
        // Default toggle states
        toggles["dontAllowPlayed"] = true;
        toggles["subsPriority"] = false;
        toggles["subsOnly"] = true;
        toggles["liveOnly"] = false;
        toggles["botReplies"] = true;
        toggles["enableRequests"] = true;
        
        // Default tab
        currentTab = 0;
        showSettings = false;
    }
    
    void Render() {
        // Main window always visible
        RenderMainWindow();
        
        // Settings window (toggleable)
        if (showSettings) {
            RenderSettingsWindow();
        }
    }
    
private:
    std::vector<QueueEntry> queueEntries;
    std::vector<BannedUser> bannedUsers;
    std::map<std::string, bool> toggles;
    int currentTab;
    bool showSettings;
    
    void RenderMainWindow() {
        ImGui::SetNextWindowSize(ImVec2(440, 600), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiCond_FirstUseEver);
        
        ImGui::Begin("HwGDReqs by MalikHw47", nullptr, 
            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
        
        // Title
        ImGui::TextColored(ImVec4(0.94f, 0.94f, 0.98f, 1.0f), "HwGDReqs");
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.69f, 1.0f), "by MalikHw47");
        ImGui::Separator();
        ImGui::Spacing();
        
        // Queue list
        ImGui::BeginChild("QueueList", ImVec2(0, 380), true);
        
        for (auto& entry : queueEntries) {
            if (entry.title.empty()) {
                ImGui::TextColored(ImVec4(0.43f, 0.43f, 0.54f, 0.7f), "— empty slot —");
                continue;
            }
            
            // Main line with title and expand button
            ImGui::PushID(&entry);
            
            ImGui::TextColored(ImVec4(0.92f, 0.92f, 0.96f, 1.0f), "%s", entry.title.c_str());
            ImGui::SameLine(ImGui::GetWindowWidth() - 40);
            
            std::string expandLabel = entry.expanded ? "▼" : "▶";
            if (ImGui::SmallButton(expandLabel.c_str())) {
                entry.expanded = !entry.expanded;
            }
            
            // Subtitle
            std::string subtitle = entry.requester + " - " + entry.creator;
            ImGui::TextColored(ImVec4(0.69f, 0.69f, 0.8f, 1.0f), "%s", subtitle.c_str());
            
            // Expandable actions
            if (entry.expanded) {
                ImGui::Separator();
                ImGui::Spacing();
                
                if (ImGui::Button("Delete", ImVec2(60, 0))) {
                    // Delete entry
                    entry.title = "";
                    entry.requester = "";
                    entry.creator = "";
                }
                ImGui::SameLine();
                
                if (ImGui::Button("Ban Requester", ImVec2(100, 0))) {
                    // Ban requester
                    if (!entry.requester.empty()) {
                        bannedUsers.push_back({entry.requester, true});
                    }
                }
                ImGui::SameLine();
                
                if (ImGui::Button("Ban Creator", ImVec2(90, 0))) {
                    // Ban creator
                    if (!entry.creator.empty()) {
                        bannedUsers.push_back({entry.creator, false});
                    }
                }
                
                ImGui::Spacing();
            }
            
            ImGui::PopID();
            ImGui::Separator();
        }
        
        ImGui::EndChild();
        ImGui::Spacing();
        
        // Controls row
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.15f, 0.15f, 0.23f, 1.0f));
        ImGui::BeginChild("Controls", ImVec2(0, 60), true);
        
        // Enable requests toggle
        bool* enableRequests = &toggles["enableRequests"];
        ImGui::Text("enable requests");
        ImGui::SameLine();
        ImGui::Checkbox("##EnableRequests", enableRequests);
        
        ImGui::SameLine(ImGui::GetWindowWidth() - 220);
        
        // Buttons
        if (ImGui::Button("random", ImVec2(60, 0))) {
            // Random button action
        }
        ImGui::SameLine();
        
        if (ImGui::Button("settings", ImVec2(60, 0))) {
            showSettings = !showSettings;
        }
        ImGui::SameLine();
        
        if (ImGui::Button("Clear queue", ImVec2(80, 0))) {
            // Clear all non-empty entries
            for (auto& entry : queueEntries) {
                if (!entry.title.empty()) {
                    entry.title = "";
                    entry.requester = "";
                    entry.creator = "";
                }
            }
        }
        
        ImGui::EndChild();
        ImGui::PopStyleColor();
        
        ImGui::End();
    }
    
    void RenderSettingsWindow() {
        ImGui::SetNextWindowSize(ImVec2(440, 500), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImVec2(560, 100), ImGuiCond_FirstUseEver);
        
        ImGui::Begin("HwGDReqs Settings", &showSettings,
            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
        
        // Title
        ImGui::TextColored(ImVec4(0.94f, 0.94f, 0.98f, 1.0f), "HwGDReqs");
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.69f, 1.0f), "Settings");
        ImGui::Separator();
        ImGui::Spacing();
        
        // Login/Logout buttons
        if (ImGui::Button("login", ImVec2(60, 0))) {
            // Login action
        }
        ImGui::SameLine();
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.45f);
        ImGui::Button("logout", ImVec2(60, 0));
        ImGui::PopStyleVar();
        ImGui::Spacing();
        
        // Toggles
        RenderToggle("don't allow already played levels", "dontAllowPlayed");
        RenderToggle("Subs priority (twitch)", "subsPriority");
        RenderToggle("subs only (twitch)", "subsOnly");
        RenderToggle("live only (twitch)", "liveOnly");
        RenderToggle("bot replies (twitch)", "botReplies");
        
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        
        // Tabs
        if (ImGui::Button("banned requesters", ImVec2(150, 0))) {
            currentTab = 0;
        }
        ImGui::SameLine();
        if (ImGui::Button("banned creators", ImVec2(150, 0))) {
            currentTab = 1;
        }
        
        ImGui::Spacing();
        ImGui::Separator();
        
        // Tab content
        ImGui::BeginChild("BannedList", ImVec2(0, 180), true);
        
        if (currentTab == 0) {
            // Show banned requesters
            for (const auto& user : bannedUsers) {
                if (user.isRequester) {
                    ImGui::Text("%s", user.name.c_str());
                    ImGui::SameLine(ImGui::GetWindowWidth() - 70);
                    if (ImGui::SmallButton("unban")) {
                        // Unban action
                    }
                }
            }
            size_t requesterCount = std::count_if(bannedUsers.begin(), bannedUsers.end(), 
                [](const BannedUser& u) { return u.isRequester; });
            ImGui::TextColored(ImVec4(0.43f, 0.43f, 0.54f, 1.0f), 
                "(%zu banned requesters)", requesterCount);
        } else {
            // Show banned creators
            for (const auto& user : bannedUsers) {
                if (!user.isRequester) {
                    ImGui::Text("🎮 %s", user.name.c_str());
                    ImGui::SameLine(ImGui::GetWindowWidth() - 70);
                    if (ImGui::SmallButton("unban")) {
                        // Unban action
                    }
                }
            }
            size_t creatorCount = std::count_if(bannedUsers.begin(), bannedUsers.end(), 
                [](const BannedUser& u) { return !u.isRequester; });
            ImGui::TextColored(ImVec4(0.43f, 0.43f, 0.54f, 1.0f), 
                "(%zu banned creators)", creatorCount);
        }
        
        ImGui::EndChild();
        
        ImGui::End();
    }
    
    void RenderToggle(const char* label, const std::string& key) {
        ImGui::Text("%s", label);
        ImGui::SameLine();
        ImGui::Checkbox(("##" + key).c_str(), &toggles[key]);
    }
};

// Main application entry point (SDL2 + OpenGL2 - exactly like your working app)
int main(int argc, char* argv[]) {
    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS) != 0) {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    // Setup window
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window* window = SDL_CreateWindow("HwGDReqs", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Setup Dear ImGui style (matching your HTML mockup)
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 0.0f;
    style.ChildRounding = 0.0f;
    style.FrameRounding = 0.0f;
    style.ScrollbarRounding = 0.0f;
    style.GrabRounding = 0.0f;
    style.TabRounding = 0.0f;
    
    // Custom dark theme matching HTML mockup
    ImVec4* colors = style.Colors;
    colors[ImGuiCol_WindowBg] = ImVec4(0.18f, 0.18f, 0.25f, 1.0f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.13f, 0.13f, 0.20f, 1.0f);
    colors[ImGuiCol_Border] = ImVec4(0.29f, 0.29f, 0.38f, 1.0f);
    colors[ImGuiCol_Text] = ImVec4(0.91f, 0.91f, 0.94f, 1.0f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.43f, 0.43f, 0.54f, 1.0f);
    colors[ImGuiCol_Button] = ImVec4(0.23f, 0.23f, 0.34f, 1.0f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.31f, 0.31f, 0.44f, 1.0f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.16f, 0.16f, 0.26f, 1.0f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.42f, 0.55f, 1.0f, 1.0f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.42f, 0.55f, 1.0f, 1.0f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.42f, 0.55f, 1.0f, 1.0f);
    colors[ImGuiCol_Separator] = ImVec4(0.29f, 0.29f, 0.38f, 1.0f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.13f, 0.13f, 0.20f, 1.0f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.18f, 0.18f, 0.27f, 1.0f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.23f, 0.23f, 0.34f, 1.0f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.18f, 0.18f, 0.25f, 1.0f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.18f, 0.18f, 0.25f, 1.0f);

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL2_Init();

    // Application instance
    HwGDReqsApp app;

    // Main loop
    bool done = false;
    while (!done) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                done = true;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // Render application
        app.Render();

        // Render
        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(0.12f, 0.12f, 0.18f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

        SDL_GL_SwapWindow(window);
    }

    // Cleanup
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
