#include "snippets.h"
#include "ImGuiColorTextEdit/TextEditor.h"
#include "imgui.h"
#include "hello_imgui/icons_font_awesome.h"
#include "immapp/code_utils.h"
#include "immapp/clock.h"
#include "imgui_md_wrapper/imgui_md_wrapper.h"
#ifdef __EMSCRIPTEN__
#include "immapp/js_clipboard_tricks.h"
#endif

#include <map>
#include "fplus/fplus.hpp"


namespace Snippets
{
    void _SetTheme(TextEditor& editor, SnippetTheme palette)
    {
        if (palette == SnippetTheme::Dark)
            editor.SetPalette(TextEditor::GetDarkPalette());
        else if (palette == SnippetTheme::Light)
            editor.SetPalette(TextEditor::GetLightPalette());
        else if (palette == SnippetTheme::RetroBlue)
            editor.SetPalette(TextEditor::GetRetroBluePalette());
    }

    void _SetLanguage(TextEditor& editor, SnippetLanguage lang)
    {
        if (lang == SnippetLanguage::Cpp)
            editor.SetLanguageDefinition(TextEditor::LanguageDefinition::CPlusPlus());
        else if (lang == SnippetLanguage::Hlsl)
            editor.SetLanguageDefinition(TextEditor::LanguageDefinition::HLSL());
        else if (lang == SnippetLanguage::Glsl)
            editor.SetLanguageDefinition(TextEditor::LanguageDefinition::GLSL());
        else if (lang == SnippetLanguage::C)
            editor.SetLanguageDefinition(TextEditor::LanguageDefinition::C());
        else if (lang == SnippetLanguage::Sql)
            editor.SetLanguageDefinition(TextEditor::LanguageDefinition::SQL());
        else if (lang == SnippetLanguage::AngelScript)
            editor.SetLanguageDefinition(TextEditor::LanguageDefinition::AngelScript());
        else if (lang == SnippetLanguage::Lua)
            editor.SetLanguageDefinition(TextEditor::LanguageDefinition::Lua());
        else if (lang == SnippetLanguage::Python)
            editor.SetLanguageDefinition(TextEditor::LanguageDefinition::Python());
    }

#ifdef __EMSCRIPTEN__
    void _ProcessClipboard_Emscripten(TextEditor& editor)
    {
      if (!ImGui::IsItemHovered())
          return;

      ImGuiIO& io = ImGui::GetIO();
      auto shift = io.KeyShift;
      //auto ctrl = io.ConfigMacOSXBehaviors ? io.KeySuper : io.KeyCtrl;
      // auto alt = io.ConfigMacOSXBehaviors ? io.KeyCtrl : io.KeyAlt;

      auto ctrl = io.KeySuper || io.KeyCtrl;

      bool shallFillBrowserClipboard = false;
      if (ctrl && !shift && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Insert)))
          shallFillBrowserClipboard = true;
      else if (ctrl && !shift && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_C)))
          shallFillBrowserClipboard = true;
      else if (ctrl && !shift && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_X)))
          shallFillBrowserClipboard = true;
      else if (!ctrl && shift && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Delete)))
          shallFillBrowserClipboard = true;

      if (shallFillBrowserClipboard)
          JsClipboard_SetClipboardText(editor.GetSelectedText().c_str());
    }
#endif // #ifdef __EMSCRIPTEN__


    void ShowCodeSnippet(const SnippetData& snippetData, float width, int overrideHeightInLines)
    {
        if (width == 0.f)
            width = (ImGui::GetContentRegionMax().x  - ImGui::GetWindowContentRegionMin().x - ImGui::GetStyle().ItemSpacing.x);

        auto id = ImGui::GetID(snippetData.Code.c_str());
        ImGui::PushID(id);
        static std::map<ImGuiID, TextEditor> gEditors;
        static std::map<ImGuiID, double> timeClickCopyButton;

        if (! fplus::map_contains(gEditors, id))
        {
            gEditors.insert({id, TextEditor()});
            auto& editor = gEditors.at(id);
            _SetLanguage(editor, snippetData.Language);
            _SetTheme(editor, snippetData.Palette);
        }

        auto& editor = gEditors.at(id);
        if (editor.GetText().empty() || snippetData.ReadOnly)
        {
            std::string displayedCode = snippetData.DeIndentCode ? CodeUtils::UnindentCode(snippetData.Code) : snippetData.Code;
            displayedCode += "\n"; // add final empty line, so that the editor shows all
            if (editor.GetText() != displayedCode)
                editor.SetText(displayedCode);
        }

        ImGui::BeginGroup();

        // Title Line
        bool hasTitleLine = ! snippetData.DisplayedFilename.empty() || snippetData.ShowCopyButton || snippetData.ShowCursorPosition;

        float em = ImGui::GetFontSize();

        ImVec2 editorSize;
        {
            editorSize.x = width;

            int nbVisibleLines = 0;
            if ((snippetData.HeightInLines == 0) && (overrideHeightInLines==0))
                nbVisibleLines = fplus::count('\n', snippetData.Code) + 1;
            else if (overrideHeightInLines != 0)
                nbVisibleLines = overrideHeightInLines;
            else
                nbVisibleLines = snippetData.HeightInLines;

            if ((snippetData.MaxHeightInLines > 0) && (nbVisibleLines > snippetData.MaxHeightInLines))
                nbVisibleLines = snippetData.MaxHeightInLines;
            editorSize.y = em * nbVisibleLines;
        }

        if (hasTitleLine)
        {
            ImVec2 topLeft = ImGui::GetCursorPos();
            ImVec2 topRight { topLeft.x + editorSize.x, topLeft.y};
            float textY = topRight.y + em * 0.2;

            if (! snippetData.DisplayedFilename.empty())
            {
                ImGui::SetCursorPos({topLeft.x, textY});
                ImGui::Text("%s", snippetData.DisplayedFilename.c_str());
            }

            if (snippetData.ShowCursorPosition)
            {
                float textX = snippetData.ShowCopyButton ? topRight.x - em * 6.f : topRight.x - em * 4.5f;
                ImGui::SetCursorPos({textX, textY});
                auto pos = editor.GetCursorPosition();
                ImGui::Text("L:%02i C:%02i", pos.mLine + 1, pos.mColumn + 1);
            }

            if (snippetData.ShowCopyButton)
            {
                ImGui::SetCursorPos({topRight.x - em * 1.5f, topRight.y});
                if (ImGui::Button(ICON_FA_COPY))
                {
                    timeClickCopyButton[id] = ImmApp::ClockSeconds();
                    ImGui::SetClipboardText(snippetData.Code.c_str());
                    #ifdef __EMSCRIPTEN__
                    JsClipboard_SetClipboardText(snippetData.Code.c_str());
                    #endif
                }

                bool wasCopiedRecently = false;
                if (fplus::map_contains(timeClickCopyButton, id))
                {
                    double now = ImmApp::ClockSeconds();
                    double deltaTime = now - timeClickCopyButton.at(id);
                    if (deltaTime < 0.7)
                        wasCopiedRecently = true;
                }
                if (wasCopiedRecently)
                    ImGui::SetTooltip("Copied!");
                else if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("Copy");
            }

            ImGui::SetCursorPos(topRight);
            ImGui::NewLine();
        }

        ImGui::PushFont(ImGuiMd::GetCodeFont());
        editor.Render(std::to_string(id).c_str(), editorSize, snippetData.Border);
#ifdef __EMSCRIPTEN__
        _ProcessClipboard_Emscripten(editor);
#endif

        ImGui::PopFont();
        ImGui::EndGroup();
        ImGui::PopID();
    }


    float _EditorWidth(int nbSideBySideEditors)
    {
        float margins_x = (nbSideBySideEditors + 1) * ImGui::GetStyle().ItemSpacing.x;
        float windowContentWidth = ImGui::GetContentRegionMax().x  - ImGui::GetWindowContentRegionMin().x;
        float editorWidth= (windowContentWidth - margins_x) / nbSideBySideEditors;
        return editorWidth;
    }

    void ShowSideBySideSnippets(const std::vector<SnippetData>& snippets , bool hideIfEmpty, bool equalVisibleLines)
    {
        int nbSideBySideEditors = snippets.size();

        if (hideIfEmpty)
        {
            for (const auto& snippet: snippets)
                if (snippet.Code.empty())
                    nbSideBySideEditors -= 1;
            if (nbSideBySideEditors == 0)
                return;
        }

        int overrideHeightInLines = 0;
        if (equalVisibleLines)
        {
            auto linesPerSnippets = fplus::transform([](const SnippetData& s) {return fplus::count('\n', s.Code);}, snippets);
            overrideHeightInLines = fplus::maximum(linesPerSnippets) + 1;
        }

        float editorWidth = _EditorWidth(nbSideBySideEditors);

        for (const auto& snippet: snippets)
        {
            bool show = !hideIfEmpty || !snippet.Code.empty();
            if (show)
            {
                ShowCodeSnippet(snippet, editorWidth, overrideHeightInLines);
                ImGui::SameLine();
            }
        }
        ImGui::NewLine();
    }


    void ShowSideBySideSnippets(const SnippetData& snippet1, const SnippetData& snippet2, bool hideIfEmpty, bool equalVisibleLines)
    {
        ShowSideBySideSnippets({snippet1, snippet2}, hideIfEmpty, equalVisibleLines);
    }
}