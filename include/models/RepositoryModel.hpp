#pragma once

// Library headers
#include <git2.h>
#include <wx/dataview.h>

// C++ headers
#include <cstdint>
#include <filesystem>
#include <vector>

class RepositoryModel : public wxDataViewModel {
  public:
  typedef wxDataViewItem Data;
  struct InternalData {
    std::filesystem::path folderPath{};
    std::shared_ptr< git_repository > gitRepo = nullptr;
  };

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
  void ScanPath( std::filesystem::path const& path );

  private:
  std::filesystem::path basePath_{};
  std::vector< Data > items_{};
};
