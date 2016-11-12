#include "stdafx.h"
#include "BackupInfoDlg.h"
#include "utbackup/propertypool/IPropertyPool.h"
#include "common/util/File.h"
#include "common/util/FilePath.h"

using namespace FileUtil;

//----------------------------------------------------------------------------
// Class definition - public members.
BackupInfoDlg::BackupInfoDlg(IPropertyPool & aPropPool)
 : mPropPool(aPropPool)
{

}

//----------------------------------------------------------------------------
// Class definition - private members.
BOOL BackupInfoDlg::OnInitDialog(CWindow /*aWndFocus*/, LPARAM /*aInitParam*/)
{
  try
  {
    // init controls
    mTree.Attach(GetDlgItem(IDC_TREE_INFO));
    mFilterCombo.Attach(GetDlgItem(IDC_COMBO_FILTER));
    mFilterEdit.Attach(GetDlgItem(IDC_EDIT_FILTER));

    DlgResize_Init(true);

    PopulateFilterCombo();

    // decode the project file to get the list of torrents
    DecodeProjectFile();

    // populate the tree
    PopulateInfoTree();
  }
  CATCH_EVERYTHING;

  return TRUE;
}

LRESULT BackupInfoDlg::OnOk(WORD /*aNotifyCode*/, WORD /*aID*/, HWND /*aWindow*/, BOOL & /*aHandled*/)
{
  try
  {
    EndDialog(IDOK);
  }
  CATCH_EVERYTHING;

  return TRUE;
}

void BackupInfoDlg::PopulateFilterCombo()
{
  mFilterCombo.Clear();

  mFilterCombo.AddString(L"Label");
  mFilterCombo.AddString(L"Name");

  mFilterCombo.SetCurSel(0);
}

void BackupInfoDlg::DecodeProjectFile()
{
  // decode the project file and get the resume.dat file content
  const wstring & projectFilePath = mPropPool.GetProperty(UTB_BACKUPPATH_PROP);
  if ( ! ::PathFileExists(projectFilePath.c_str()) )
    throw Exception(NULL, L"The project file does not exist.");

  File projFile(projectFilePath, false, CharsetEncoding::Utf8);
  wstring content = projFile.Read();
  if (content.empty())
    throw Exception(*this, L"Invalid project file.");

  if (!mbManager.Decode2(content))
    throw Exception(*this, L"This project file is corrupted.");

  auto root = mbManager.GetRootItem();
  auto packOption = root->GetChild(UTB_BACKUPPACKFILES_PROP);
  if (!packOption)
    throw Exception(*this, L"This project file is corrupted.");

  bool packedFiles = packOption->GetValue() == PROP_TRUE;

  wstring resumeFileContent;
  if (packedFiles)
  {
    auto files = root->GetChild(L"Files", bEncodeItem::Dictionary);
    if (!files)
      throw Exception(*this, L"This project file is corrupted.");

    auto resumeFile = files->GetChild(L"resume.dat");
    if (!resumeFile)
      throw Exception(*this, L"This project file is corrupted.");

    resumeFileContent = resumeFile->GetValue();
  }
  else
  {

  }

  // decode resume file
  if (resumeFileContent.empty())
    throw Exception(*this, L"Could not retrieve torrent list.");

  mbManager.PurgeItems();

  if (!mbManager.Decode2(resumeFileContent))
    throw Exception(*this, L"Could not decode the torrent list.");

  // remove non-torrent nodes
  root = mbManager.GetRootItem();
  auto node = root->GetChild(L".fileguard");
  if (node)
    node->Release();

  node = root->GetChild(L"rec", bEncodeItem::List);
  if (node)
    node->Release();
}

void BackupInfoDlg::PopulateInfoTree()
{
  // get filter type
  FilterType filter = static_cast<FilterType>(mFilterCombo.GetCurSel());

  // get filter text
  CString windowText;
  mFilterEdit.GetWindowText(windowText);
  wstring filterText = windowText.GetString();
    
  // get a list with the matching torrents
  vector<bEncodeItem *> torrents;
  auto root = mbManager.GetRootItem();
  auto it = root->ChildrenBegin();
  auto end = root->ChildrenEnd();
  for (; it != end; ++it)
  {
    if (MatchesFilter(*it, filter, filterText))
    {
      torrents.push_back(*it);
    }
  }

  // sort them
  std::sort(torrents.begin(), torrents.end(), [](bEncodeItem * aLeft, bEncodeItem * aRight) -> bool
  {
    auto lNode = aLeft->GetChild(L"caption");
    auto rNode = aRight->GetChild(L"caption");
    if (lNode && rNode)
    {
      return lNode->GetValue() < rNode->GetValue();
    }

    return true;
  });

  // reset tree content
  mTree.DeleteAllItems();

  // add tree root
  wstring rootText = L"Torrent items (" + StringUtil::IntToString(torrents.size()) + L")";
  HTREEITEM treeRoot = mTree.InsertItem(rootText.c_str(), TVI_ROOT, TVI_FIRST);

  // populate the tree
  for (auto & torrent : torrents)
  {
    auto node = torrent->GetChild(L"caption");
    if (node)
    {
      HTREEITEM item = mTree.InsertItem(node->GetValue().c_str(), treeRoot, TVI_LAST);

      auto prop = torrent->GetChild(L"path"); // add path
      if (prop)
      {
        wstring text = prop->GetLabel() + L" = " + prop->GetValue();
        mTree.InsertItem(text.c_str(), item, TVI_LAST);
      }

      prop = torrent->GetChild(L"rootdir"); // add rootdir
      if (prop)
      {
        wstring text = prop->GetLabel() + L" = " + prop->GetValue();
        mTree.InsertItem(text.c_str(), item, TVI_LAST);
      }

      prop = torrent->GetChild(L"labels", bEncodeItem::List); // add labels
      if (prop)
      {
        vector<wstring> labels;
        for (auto lbIt = prop->ChildrenBegin(); lbIt != prop->ChildrenEnd(); ++lbIt)
          labels.push_back((*lbIt)->GetLabel());

        wstring text = prop->GetLabel() + L" = " + StringUtil::Merge(labels, L", ");;
        mTree.InsertItem(text.c_str(), item, TVI_LAST);
      }
    }
  }

  // expand root item
  mTree.Expand(treeRoot, TVE_EXPAND);
}

bool BackupInfoDlg::MatchesFilter(bEncodeItem * aItem, FilterType aFilterType, 
                                  const wstring & aFilterText)
{
  if (!aFilterText.empty())
  {
    if (aFilterType == Name)
    {
      wstring itemName = aItem->GetLabel();
      
      if (itemName.find(aFilterText) == wstring::npos)
        return false;
    }
  }

  return true;
}
