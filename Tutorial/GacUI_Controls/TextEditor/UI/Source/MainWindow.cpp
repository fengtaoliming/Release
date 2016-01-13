﻿/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
GacUI::MainWindow

This file is generated by: Vczh GacUI Resource Code Generator
***********************************************************************/

#include "Demo.h"

using namespace vl::stream;

namespace demo
{
	class XmlColorizer : public GuiTextBoxRegexColorizer
	{
	public:
		XmlColorizer()
		{
			text::ColorEntry entry=GetCurrentTheme()->GetDefaultTextBoxColorEntry();
			SetDefaultColor(entry);

			entry.normal.text=Color(0, 128, 0);
			AddToken(L"/<!--([^/-]|-[^/-]|--[^>])*--/>", entry);

			entry.normal.text=Color(128, 0, 255);
			AddToken(L"/<!/[CDATA/[([^/]]|/][^/]]|/]/][^>])*/]/]/>", entry);

			entry.normal.text=Color(0, 0, 0);
			AddToken(L"\"[^\"]*\"", entry);

			entry.normal.text=Color(0, 0, 255);
			AddToken(L"[<>=]", entry);

			entry.normal.text=Color(255, 0, 0);
			AddToken(L"[a-zA-Z0-9_/-:]+", entry);

			entry.normal.text=Color(163, 21, 21);
			AddExtraToken(entry);

			Setup();
		}

		void ColorizeTokenContextSensitive(int lineIndex, const wchar_t* text, vint start, vint length, vint& token, int& contextState)override
		{
			// 0 < 1 name 2 att > 0
			switch(token)
			{
			case 3:
				if(length==1)
				{
					switch(text[start])
					{
					case '<':
						contextState=1;
						break;
					case '>':
						contextState=0;
						break;
					}
				}
				break;
			case 4:
				switch(contextState)
				{
				case 0:
					token=-1;
					break;
				case 1:
					token=5;
					contextState=2;
					break;
				}
				break;
			}
		}

		vint GetContextStartState()override
		{
			return 0;
		}
	};
}

namespace demo
{
	// #region CLASS_MEMBER_GUIEVENT_HANDLER (DO NOT PUT OTHER CONTENT IN THIS #region.)

	void MainWindow::commandAbout_Executed(GuiGraphicsComposition* sender, vl::presentation::compositions::GuiEventArgs& arguments)
	{
		auto window = new AboutWindow;
		window->MoveToScreenCenter();
		window->ShowModalAndDelete(this, [](){});
	}

	void MainWindow::commandEditCopy_Executed(GuiGraphicsComposition* sender, vl::presentation::compositions::GuiEventArgs& arguments)
	{
		textBox->Copy();
	}

	void MainWindow::commandEditCut_Executed(GuiGraphicsComposition* sender, vl::presentation::compositions::GuiEventArgs& arguments)
	{
		textBox->Cut();
	}

	void MainWindow::commandEditDelete_Executed(GuiGraphicsComposition* sender, vl::presentation::compositions::GuiEventArgs& arguments)
	{
		textBox->SetSelectionText(L"");
	}

	void MainWindow::commandEditFind_Executed(GuiGraphicsComposition* sender, vl::presentation::compositions::GuiEventArgs& arguments)
	{
		findWindow->Show();
	}

	void MainWindow::commandEditPaste_Executed(GuiGraphicsComposition* sender, vl::presentation::compositions::GuiEventArgs& arguments)
	{
		textBox->Paste();
	}

	void MainWindow::commandEditRedo_Executed(GuiGraphicsComposition* sender, vl::presentation::compositions::GuiEventArgs& arguments)
	{
		textBox->Redo();
	}

	void MainWindow::commandEditSelect_Executed(GuiGraphicsComposition* sender, vl::presentation::compositions::GuiEventArgs& arguments)
	{
		textBox->SelectAll();
	}

	void MainWindow::commandEditUndo_Executed(GuiGraphicsComposition* sender, vl::presentation::compositions::GuiEventArgs& arguments)
	{
		textBox->Undo();
	}

	void MainWindow::commandFileExit_Executed(GuiGraphicsComposition* sender, vl::presentation::compositions::GuiEventArgs& arguments)
	{
		Close();
	}

	void MainWindow::commandFileNewText_Executed(GuiGraphicsComposition* sender, vl::presentation::compositions::GuiEventArgs& arguments)
	{
		if (CanCloseFile())
		{
			textBox->SetText(L"");
			textBox->ClearUndoRedo();
			SetupTextConfig();
		}
	}

	void MainWindow::commandFileNewXml_Executed(GuiGraphicsComposition* sender, vl::presentation::compositions::GuiEventArgs& arguments)
	{
		if (CanCloseFile())
		{
			textBox->SetText(L"");
			textBox->ClearUndoRedo();
			SetupXmlConfig();
		}
	}

	void MainWindow::commandFileOpenText_Executed(GuiGraphicsComposition* sender, vl::presentation::compositions::GuiEventArgs& arguments)
	{
		OpenFile(0);
	}

	void MainWindow::commandFileOpenXml_Executed(GuiGraphicsComposition* sender, vl::presentation::compositions::GuiEventArgs& arguments)
	{
		OpenFile(1);
	}

	void MainWindow::commandFileOpen_Executed(GuiGraphicsComposition* sender, vl::presentation::compositions::GuiEventArgs& arguments)
	{
		OpenFile(dialogOpen->GetFilterIndex());
	}

	void MainWindow::commandFileSaveAs_Executed(GuiGraphicsComposition* sender, vl::presentation::compositions::GuiEventArgs& arguments)
	{
		SaveFile(true);
	}

	void MainWindow::commandFileSave_Executed(GuiGraphicsComposition* sender, vl::presentation::compositions::GuiEventArgs& arguments)
	{
		SaveFile(false);
	}

	void MainWindow::window_Closing(GuiGraphicsComposition* sender, vl::presentation::compositions::GuiRequestEventArgs& arguments)
	{
		arguments.cancel = !CanCloseFile();
	}

	// #endregion CLASS_MEMBER_GUIEVENT_HANDLER

	bool MainWindow::CanCloseFile()
	{
		if (textBox->GetModified())
		{
			switch (dialogQueryClose->ShowDialog())
			{
			case INativeDialogService::MessageBoxButtonsOutput::SelectYes:
				return SaveFile(false);
			case INativeDialogService::MessageBoxButtonsOutput::SelectNo:
				return true;
			default:
				return false;
			}
		}
		else
		{
			return true;
		}
	}

	bool MainWindow::OpenFile(vint filterIndex)
	{
		if (CanCloseFile())
		{
			dialogOpen->SetFilterIndex(filterIndex);
			if (dialogOpen->ShowDialog())
			{
				FileStream fileStream(dialogOpen->GetFileName(), FileStream::ReadOnly);
				if (fileStream.IsAvailable())
				{
					BomDecoder decoder;
					DecoderStream decoderStream(fileStream, decoder);
					StreamReader reader(decoderStream);
					textBox->SetText(reader.ReadToEnd());
					textBox->Select(TextPos(), TextPos());
					textBox->SetFocus();
					textBox->ClearUndoRedo();

					fileName = dialogOpen->GetFileName();
					if (INVLOC.EndsWith(fileName, L".xml", Locale::IgnoreCase))
					{
						SetupXmlConfig();
					}
					else
					{
						SetupTextConfig();
					}
					return true;
				}
				else
				{
					dialogCannotOpen->ShowDialog();
				}
			}
		}
		return false;
	}

	bool MainWindow::SaveFile(bool saveAs)
	{
		WString targetFileName = fileName;
		if (saveAs || targetFileName == L"")
		{
			dialogSave->SetFilterIndex(isXml ? 1 : 0);
			if (dialogSave->ShowDialog())
			{
				targetFileName = dialogSave->GetFileName();
			}
			else
			{
				return false;
			}
		}

		FileStream fileStream(targetFileName, FileStream::WriteOnly);
		if (fileStream.IsAvailable())
		{
			BomEncoder encoder(BomEncoder::Utf16);
			EncoderStream encoderStream(fileStream, encoder);
			StreamWriter writer(encoderStream);
			writer.WriteString(textBox->GetText());
			textBox->NotifyModificationSaved();

			fileName = targetFileName;
			if (INVLOC.EndsWith(fileName, L".xml", Locale::IgnoreCase))
			{
				SetupXmlConfig();
			}
			else
			{
				SetupTextConfig();
			}
			return true;
		}
		else
		{
			dialogCannotSave->ShowDialog();
		}
		return false;
	}

	void MainWindow::SetupTextConfig()
	{
		if (isXml == true)
		{
			isXml = false;
			textBox->SetColorizer(nullptr);
		}
	}

	void MainWindow::SetupXmlConfig()
	{
		if (isXml == false)
		{
			isXml = true;
			textBox->SetColorizer(new XmlColorizer);
		}
	}

	MainWindow::MainWindow()
	{
		InitializeComponents();
		findWindow = MakePtr<FindWindow>(MakePtr<FindWindowViewModel>(textBox));
		findWindow->MoveToScreenCenter();
		findWindow->GetNativeWindow()->SetParent(GetNativeWindow());
	}

	MainWindow::~MainWindow()
	{
		ClearSubscriptions();
	}

	MainWindow::FindWindowViewModel::FindWindowViewModel(GuiMultilineTextBox* _textBox)
		:textBox(_textBox)
	{
	}

	bool MainWindow::FindWindowViewModel::FindNext(vl::WString toFind, bool caseSensitive, bool down)
	{
		auto position = textBox->GetCaretEnd();
		auto rowCount = textBox->GetRowCount();
		auto normalization = caseSensitive ? Locale::None : Locale::IgnoreCase;

		for (vint i = position.row; 0 <= i&&i < rowCount; (down ? i++ : i--))
		{
			auto rowText = textBox->GetRowText(i);
			auto fragment = rowText;
			if (i == position.row)
			{
				if (down)
				{
					fragment = rowText.Right(rowText.Length() - position.column);
				}
				else
				{
					fragment = rowText.Left(position.column);
				}
			}

			auto pair = down
				? INVLOC.FindFirst(fragment, toFind, normalization)
				: INVLOC.FindLast(fragment, toFind, normalization)
				;

			if (pair.key != -1)
			{
				TextPos begin, end;
				if (i == position.row && down)
				{
					begin = TextPos(i, pair.key + position.column);
				}
				else
				{
					begin = TextPos(i, pair.key);
				}

				if (down)
				{
					end = TextPos(begin.row, begin.column + pair.value);
				}
				else
				{
					end = TextPos(begin.row, begin.column);
					begin.column += pair.value;
				}

				textBox->Select(begin, end);
				textBox->SetFocus();
				return true;
			}
		}

		return false;
	}
}
