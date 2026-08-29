#pragma once

// Project headers
#include <models/DiffData.hpp>
#include <models/RepositoryData.hpp>

// Library headers
#include <wx/dataview.h>

// C++ headers
#include <cstdint>
#include <vector>

class DiffModel : public wxDataViewModel {
  public:
  typedef wxItemId< DiffData* > Data;
  enum Columns { ALL = -1, Path, Status, LAST };

  public:
  DiffModel();

  void SetRepository( RepositoryData* repository );
  std::shared_ptr< git_diff > GetDiff() const;

  // overrides
  public:
  virtual void GetValue( wxVariant& out_variant, wxDataViewItem const& wxdvItem, uint32_t col ) const override;
  virtual bool HasValue( wxDataViewItem const& wxdvItem, uint32_t col ) const override;
  virtual bool SetValue( wxVariant const& variant, wxDataViewItem const& wxdvItem, uint32_t col ) override;
  virtual bool GetAttr( wxDataViewItem const& wxdvItem, uint32_t col, wxDataViewItemAttr& out_attr ) const override;
  virtual bool IsEnabled( wxDataViewItem const& wxdvItem, uint32_t col ) const override;
  virtual wxDataViewItem GetParent( wxDataViewItem const& wxdvItem ) const override;
  virtual bool IsContainer( wxDataViewItem const& wxdvItem ) const override;
  virtual bool HasContainerColumns( wxDataViewItem const& wxdvItem ) const override;
  virtual uint32_t GetChildren( wxDataViewItem const& wxdvItem, wxDataViewItemArray& out_children ) const override;
  virtual void Resort() override;
  virtual int32_t Compare( wxDataViewItem const& wxdvItem1, wxDataViewItem const& wxdvItem2, uint32_t column, bool ascending ) const override;
  virtual bool HasDefaultCompare() const override;
  virtual bool IsListModel() const override;
  virtual bool IsVirtualListModel() const override;

  protected:
  static int GitDiffFileForeach( git_diff_delta const* delta, float progress, void* user );
  void PrintGitError( std::string const& type, int errorCode ) const;

  private:
  RepositoryData* repository_ = nullptr;
  std::shared_ptr< git_diff > diff_ = nullptr;
  size_t sortId_ = 0;
  std::vector< Data > items_{};
};
