/*==================================================================================
    Copyright (c) 2008, binaryzebra
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    * Neither the name of the binaryzebra nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE binaryzebra AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL binaryzebra BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=====================================================================================*/


#include "UI/UITextField2.h"
#include "UI/UIControlSystem.h"
#include "UI/UIStaticText.h"
#include "Input/InputSystem.h"
#include "Input/KeyboardDevice.h"

#include "UI/Private/IUITextField2Impl.h"
#include "UI/Private/UITextField2StbBind.h"

#if defined(__DAVAENGINE_WIN32__)
#include "UI/Private/Win32/UITextField2Impl.h"
#elif defined(__DAVAENGINE_IOS__)
#include "UI/Private/iOS/UITextField2Impl.h"
#elif defined(__DAVAENGINE_MACOS__)
#include "UI/Private/MacOS/UITextField2Impl.h"
#elif defined(__DAVAENGINE_WIN_UAP__)
#include "UI/Private/UWP/UITextField2Impl.h"
#elif defined(__DAVAENGINE_ANDROID__)
#include "UI/Private/Android/UITextField2Impl.h"
#endif


// Use NO_REQUIRED_SIZE to notify textFieldImpl->SetText that we don't want
// to enable of any kind of static text fitting
static const DAVA::Vector2 NO_REQUIRED_SIZE = DAVA::Vector2(-1, -1);


////////////////////////////////////////////////////////////////////////////////

namespace DAVA
{
UITextField2::UITextField2(const Rect& rect)
    : UIControl(rect)
    , staticText(new UIStaticText(Rect(Vector2(0, 0), GetSize())))
    , stb_struct(new StbTextStruct())
    , pImpl(new UITextField2Impl(this))
{
    stb_struct->field = this;
    stb_textedit_initialize_state(&stb_struct->state, 0);

    AddControl(staticText);
    SetupDefaults();
}

UITextField2::~UITextField2()
{
    SafeRelease(staticText);
    SafeDelete(stb_struct);
    SafeDelete(pImpl);
    UIControl::RemoveAllControls();
}

void UITextField2::SetupDefaults()
{
    // Control props
    SetInputEnabled(true, false);
    // Keyboard props
    SetAutoCapitalizationType(UITextField::AUTO_CAPITALIZATION_TYPE_SENTENCES);
    SetAutoCorrectionType(UITextField::AUTO_CORRECTION_TYPE_DEFAULT);
    SetSpellCheckingType(UITextField::SPELL_CHECKING_TYPE_DEFAULT);
    SetKeyboardAppearanceType(UITextField::KEYBOARD_APPEARANCE_DEFAULT);
    SetKeyboardType(UITextField::KEYBOARD_TYPE_DEFAULT);
    SetReturnKeyType(UITextField::RETURN_KEY_DEFAULT);
    SetEnableReturnKeyAutomatically(false);
    SetTextUseRtlAlign(TextBlock::RTL_DONT_USE);
    // Static text props
    SetTextAlign(ALIGN_LEFT | ALIGN_VCENTER);
    SetFontSize(26);
    // Text field props
    SetMaxLength(-1);
    SetPassword(false);
    SetText(L"");
}

UITextField2* UITextField2::Clone()
{
    UITextField2* t = new UITextField2();
    t->CopyDataFrom(this);
    return t;
}

void UITextField2::CopyDataFrom(UIControl* srcControl)
{
    UIControl::CopyDataFrom(srcControl);
    UITextField2* t = static_cast<UITextField2*>(srcControl);

    stb_struct = new StbTextStruct(*t->stb_struct);
    stb_struct->field = this;
    staticText->CopyDataFrom(t->staticText);

    cursorBlinkingTime = t->cursorBlinkingTime;
    cursorTime = t->cursorTime;
    needRedraw = t->needRedraw;
    showCursor = t->showCursor;
    SetPassword(t->GetPassword());
    SetMultiline(t->GetMultiline());
    SetText(t->text);
    SetRect(t->GetRect());

    SetAutoCapitalizationType(t->GetAutoCapitalizationType());
    SetAutoCorrectionType(t->GetAutoCorrectionType());
    SetSpellCheckingType(t->GetSpellCheckingType());
    SetKeyboardAppearanceType(t->GetKeyboardAppearanceType());
    SetKeyboardType(t->GetKeyboardType());
    SetReturnKeyType(t->GetReturnKeyType());
    SetEnableReturnKeyAutomatically(t->IsEnableReturnKeyAutomatically());
    SetTextUseRtlAlign(t->GetTextUseRtlAlign());
    SetMaxLength(t->GetMaxLength());
}

void UITextField2::innerInsertText(uint32 position, const WideString::value_type* str, uint32 length)
{
    WideString t = GetText();
    t.insert(position, str, length);
    SetText(t);
}

void UITextField2::innerDeleteText(uint32 position, uint32 length)
{
    WideString t = GetText();
    t.erase(position, length);
    SetText(t);
}

void UITextField2::InsertText(uint32 position, const WideString& str)
{
    stb_textedit_paste(stb_struct, &stb_struct->state, str.c_str(), static_cast<int>(str.length()));
}

void UITextField2::SendChar(uint32 codePoint)
{
    stb_textedit_key(stb_struct, &stb_struct->state, codePoint);
}

const WideString& UITextField2::GetText() const
{
    return text;
}

void UITextField2::SetText(const WideString& newText)
{
    if (text != newText)
    {
        if (delegate != nullptr)
        {
            delegate->OnTextChanged(this, newText, text);
        }
        text = newText;
        staticText->SetText(text, NO_REQUIRED_SIZE);
    }
}

WideString UITextField2::GetVisibleText() const
{
    return isPassword ? WideString(GetText().length(), L'*') : GetText();
}

bool UITextField2::GetMultiline() const
{
    return isMultiline;
}

void UITextField2::SetMultiline(bool value)
{
    if (value != isMultiline)
    {
        isMultiline = value;
        staticText->SetMultiline(isMultiline);
    }
}

bool UITextField2::GetPassword() const
{
    return isPassword;
}

void UITextField2::SetPassword(bool value)
{
    if (isPassword != value)
    {
        isPassword = value;
        needRedraw = true;
    }
}

UITextField2Delegate* UITextField2::GetDelegate()
{
    return delegate;
}

void UITextField2::SetDelegate(UITextField2Delegate* newDelegate)
{
    delegate = newDelegate;
}

uint32 UITextField2::GetCursorPos()
{
    return 0;
}

void UITextField2::SetCursorPos(uint32 pos)
{
}

int32 UITextField2::GetMaxLength() const
{
    return maxLength;
}

void UITextField2::SetMaxLength(int32 newMaxLength)
{
    maxLength = Max(-1, newMaxLength); //-1 valid value
}

int32 UITextField2::GetAutoCapitalizationType() const
{
    return autoCapitalizationType;
}

void UITextField2::SetAutoCapitalizationType(int32 value)
{
    autoCapitalizationType = static_cast<UITextField::eAutoCapitalizationType>(value);
    pImpl->SetAutoCapitalizationType(autoCapitalizationType);
}

int32 UITextField2::GetAutoCorrectionType() const
{
    return autoCorrectionType;
}

void UITextField2::SetAutoCorrectionType(int32 value)
{
    autoCorrectionType = static_cast<UITextField::eAutoCorrectionType>(value);
    pImpl->SetAutoCorrectionType(autoCorrectionType);
}

int32 UITextField2::GetSpellCheckingType() const
{
    return spellCheckingType;
}

void UITextField2::SetSpellCheckingType(int32 value)
{
    spellCheckingType = static_cast<UITextField::eSpellCheckingType>(value);
    pImpl->SetSpellCheckingType(spellCheckingType);
}

int32 UITextField2::GetKeyboardAppearanceType() const
{
    return keyboardAppearanceType;
}

void UITextField2::SetKeyboardAppearanceType(int32 value)
{
    keyboardAppearanceType = static_cast<UITextField::eKeyboardAppearanceType>(value);
    pImpl->SetKeyboardAppearanceType(keyboardAppearanceType);
}

int32 UITextField2::GetKeyboardType() const
{
    return keyboardType;
}

void UITextField2::SetKeyboardType(int32 value)
{
    keyboardType = static_cast<UITextField::eKeyboardType>(value);
    pImpl->SetKeyboardType(keyboardType);
}

int32 UITextField2::GetReturnKeyType() const
{
    return returnKeyType;
}

void UITextField2::SetReturnKeyType(int32 value)
{
    returnKeyType = static_cast<UITextField::eReturnKeyType>(value);
    pImpl->SetReturnKeyType(returnKeyType);
}

bool UITextField2::IsEnableReturnKeyAutomatically() const
{
    return enableReturnKeyAutomatically;
}

void UITextField2::SetEnableReturnKeyAutomatically(bool value)
{
    enableReturnKeyAutomatically = value;
    pImpl->SetEnableReturnKeyAutomatically(enableReturnKeyAutomatically);
}

void UITextField2::OpenKeyboard()
{
    pImpl->OpenKeyboard();
}

void UITextField2::CloseKeyboard()
{
    pImpl->CloseKeyboard();
}

Font* UITextField2::GetFont() const
{
    return staticText->GetFont();
}

void UITextField2::SetFont(Font* font)
{
    staticText->SetFont(font);
}

String UITextField2::GetFontPresetName() const
{
    return staticText->GetFontPresetName();
}

void UITextField2::SetFontByPresetName(const String& presetName)
{
    staticText->SetFontByPresetName(presetName);
}

void UITextField2::SetFontSize(float32 size)
{
    //TODO: staticText->SetFontSize(size);
}

Color UITextField2::GetTextColor() const
{
    return staticText->GetTextColor();
}

void UITextField2::SetTextColor(const Color& fontColor)
{
    staticText->SetTextColor(fontColor);
}

Vector2 UITextField2::GetShadowOffset() const
{
    return staticText->GetShadowOffset();
}

void UITextField2::SetShadowOffset(const DAVA::Vector2& offset)
{
    staticText->SetShadowOffset(offset);
}

Color UITextField2::GetShadowColor() const
{
    return staticText->GetShadowColor();
}

void UITextField2::SetShadowColor(const Color& color)
{
    staticText->SetShadowColor(color);
}

int32 UITextField2::GetTextAlign() const
{
    return staticText->GetTextAlign();
}

void UITextField2::SetTextAlign(int32 align)
{
    staticText->SetTextAlign(align);
}

TextBlock::eUseRtlAlign UITextField2::GetTextUseRtlAlign() const
{
    return staticText->GetTextUseRtlAlign();
}

void UITextField2::SetTextUseRtlAlign(TextBlock::eUseRtlAlign useRtlAlign)
{
    staticText->SetTextUseRtlAlign(useRtlAlign);
}

int32 UITextField2::GetTextUseRtlAlignAsInt() const
{
    return static_cast<TextBlock::eUseRtlAlign>(GetTextUseRtlAlign());
}

void UITextField2::SetTextUseRtlAlignFromInt(int32 value)
{
    SetTextUseRtlAlign(static_cast<TextBlock::eUseRtlAlign>(value));
}

void UITextField2::OnFocused()
{
    OpenKeyboard();
}

void UITextField2::OnFocusLost(UIControl* newFocus)
{
    CloseKeyboard();
    if (delegate != nullptr)
    {
        delegate->OnLostFocus(this);
    }
}

void UITextField2::Update(float32 timeElapsed)
{
    if (this == UIControlSystem::Instance()->GetFocusedControl())
    {
        //float32 timeElapsed = SystemTimer::Instance()->FrameDelta();
        cursorTime += timeElapsed;

        if (cursorTime >= 0.5f)
        {
            cursorTime = 0;
            showCursor = !showCursor;
            needRedraw = true;
        }
    }
    else if (showCursor)
    {
        cursorTime = 0;
        showCursor = false;
        needRedraw = true;
    }

    if (!needRedraw)
    {
        return;
    }

    const WideString& txt = this->GetVisibleText();
    if (this == UIControlSystem::Instance()->GetFocusedControl())
    {
        WideString txtWithCursor = txt;
        //auto pos = stb_struct->state.cursor;
        //txtWithCursor.insert(pos, showCursor ? L"|" : L" ", 1);
        staticText->SetText(txtWithCursor, NO_REQUIRED_SIZE);
    }
    else
    {
        staticText->SetText(txt, NO_REQUIRED_SIZE);
    }
    needRedraw = false;
}

void UITextField2::Input(UIEvent* currentInput)
{
    if (nullptr == delegate)
    {
        return;
    }

    if (this != UIControlSystem::Instance()->GetFocusedControl())
        return;

    if (currentInput->phase == UIEvent::Phase::KEY_DOWN ||
        currentInput->phase == UIEvent::Phase::KEY_DOWN_REPEAT)
    {
        if (currentInput->key == Key::ENTER)
        {
            delegate->OnSubmit(this);
        }
        else if (currentInput->key == Key::ESCAPE)
        {
            delegate->OnCancel(this);
        }
        else if (currentInput->key == Key::LEFT)
        {
            SendChar(STB_TEXTEDIT_K_LEFT);
        }
        else if (currentInput->key == Key::DOWN)
        {
            SendChar(STB_TEXTEDIT_K_DOWN);
        }
        else if (currentInput->key == Key::RIGHT)
        {
            SendChar(STB_TEXTEDIT_K_RIGHT);
        }
        else if (currentInput->key == Key::UP)
        {
            SendChar(STB_TEXTEDIT_K_UP);
        }
        Logger::Error("KEY_DOWN: ch:%d vk:%d", currentInput->keyChar, currentInput->key);
    }
    else if (currentInput->phase == UIEvent::Phase::CHAR ||
             currentInput->phase == UIEvent::Phase::CHAR_REPEAT)
    {
        SendChar(currentInput->keyChar);
        Logger::Error("CHAR: ch:%d vk:%d", currentInput->keyChar, currentInput->key);
    }
    else if (currentInput->phase == UIEvent::Phase::BEGAN)
    {
        Vector2 localPoint = currentInput->point - GetPosition();
        auto pos = stb_text_locate_coord(stb_struct, localPoint.x, localPoint.y);
        Logger::Debug("MouseDown: point = %fx%f, pos = %d", localPoint.x, localPoint.y, pos);
        //stb_textedit_click(stb_struct, &stb_struct->state, localPoint.x, localPoint.y);
    }
    else if (currentInput->phase == UIEvent::Phase::DRAG)
    {
        Vector2 localPoint = currentInput->point - GetPosition();
        auto pos = stb_text_locate_coord(stb_struct, localPoint.x, localPoint.y);
        Logger::Debug("MouseDrag: point = %fx%f, pos = %d", localPoint.x, localPoint.y, pos);
        //stb_textedit_drag(stb_struct, &stb_struct->state, localPoint.x, localPoint.y);
    }

    currentInput->SetInputHandledType(UIEvent::INPUT_HANDLED_SOFT); // Drag is not handled - see please DF-2508.
}

void UITextField2::SetSize(const DAVA::Vector2& newSize)
{
    UIControl::SetSize(newSize);
    staticText->SetSize(newSize);
}

} // namespace DAVA
