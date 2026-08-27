#pragma once

// Project headers
#include <models/RepositoryData.hpp>

// Library headers
#include <wx/dataview.h>

// C++ headers
#include <cstdint>
#include <filesystem>
#include <vector>

class RepositoryModel : public wxDataViewModel {
  public:
  typedef wxItemId< RepositoryData* > Data;
  enum Columns { ALL = -1, FolderName, Depth, Path, CurrentBranch, CurrentStatus, LAST };

  public:
  RepositoryModel();

  void SetBasePath( std::filesystem::path const& basePath );

  // overrides
  public:
  virtual void GetValue( wxVariant& out_variant, wxDataViewItem const& item, uint32_t col ) const override;
  virtual bool HasValue( wxDataViewItem const& item, uint32_t col ) const override;
  virtual bool SetValue( wxVariant const& variant, wxDataViewItem const& item, uint32_t col ) override;
  virtual bool GetAttr( wxDataViewItem const& item, uint32_t col, wxDataViewItemAttr& out_attr ) const override;
  virtual bool IsEnabled( wxDataViewItem const& item, uint32_t col ) const override;
  virtual wxDataViewItem GetParent( wxDataViewItem const& item ) const override;
  virtual bool IsContainer( wxDataViewItem const& item ) const override;
  virtual bool HasContainerColumns( wxDataViewItem const& item ) const override;
  virtual uint32_t GetChildren( wxDataViewItem const& item, wxDataViewItemArray& out_children ) const override;
  virtual void Resort() override;
  virtual int32_t Compare( wxDataViewItem const& item1, wxDataViewItem const& item2, uint32_t column, bool ascending ) const override;
  virtual bool HasDefaultCompare() const override;
  virtual bool IsListModel() const override;
  virtual bool IsVirtualListModel() const override;

  private:
  void ScanPath( std::filesystem::path const& path, uint32_t depth, int64_t& sortId );
  void AddPath( std::filesystem::path const& path, uint32_t depth, int64_t& sortId );

  private:
  std::filesystem::path basePath_{};
  std::vector< Data > items_{};
};
