/***************************************************************************
                    clogfileview.h - the logical file view,draw the contents of 
		           a projectfile into a tree
                             -------------------                                         

    version              :                                   
    begin                : 20 Jul 1998                                        
    copyright            : (C) 1998 by Sandy Meier                         
    email                : smeier@rz.uni-potsdam.de                                     
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 *                                                                         *
 ***************************************************************************/
#ifndef CLOGFILEVIEW_H
#define CLOGFILEVIEW_H

#include "ctreeview.h"
#include <qptrdict.h>
#include <qstrlist.h>

class CProject;
class CTreeHandler;
class KPopupMenu;
class QListViewItem;

/** the logical-file-view,draw the contents of
  * a cprojectinfo into a tree
  *@author Sandy Meier
  */

class CLogFileView : public CTreeView {
  Q_OBJECT
public:

  /** construtor */
  CLogFileView(bool s_path=false,QWidget*parent=0,const char* name=0);

  /** destructor */
  ~CLogFileView();
    
  /** starts the refresh */
  void refresh(CProject* prj);
  /** store the internal state to the projectfile*/
  void storeState(CProject* prj);
  
  /** set the filename that will be selected after a refresh*/
  void setPreSelectedItem(const QString& rel_filename);
  /** select the first item, after a refresh*/
  void setFirstItemSelected();

  /** disabled the popupmenus, used in cfilepropdialog */
  void setPopupMenusDisabled();
  /** all groups are opened after a refresh*/
  void setAllGroupsOpened(){allgroups_opened=true;}
  /** returns the relative filename*/
  QString getFileName(QListViewItem* item);
  QString getFullFilename(QListViewItem* item);
  bool showPath(){return show_path;}

protected: // Implementations of virtual methods.

  /** Get the current popupmenu. */
  virtual KPopupMenu *getCurrentPopup();

protected:
  void split(QString str,QStrList& filters);
 protected slots:
    void slotSelectionChanged( QListViewItem *);
    void slotNewClass();
    void slotNewFile();
    void slotNewGroup();
    void slotFileProp();
    void slotFileRemove();
    void slotFileDelete();
    void slotGroupProp();
    void slotGroupRemove();
    void slotAddToRepository();
    void slotRemoveFromRepository();
    void slotUpdate();
    void slotCommit();
    /**  */
	  void slotShowPath();
    
 signals:
    void selectedNewClass();
    void selectedNewFile();
    void showFileProperties(const QString& file);
    void selectedFileRemove(QString file);
    /**
     * This signal is a helper to remove the file also from the actual editlist
     */
    void removeFileFromEditlist(const QString &absfilename);

    void selectedGroupProp();
    void logFileTreeSelected(QString);
    void menuItemHighlighted(int);
    void commitFileToVCS(QString file);
    void updateFileFromVCS(QString file);

protected:
  KPopupMenu *popup;
  bool popupmenu_disable;
  bool firstitemselect;
  bool allgroups_opened;
  bool show_path;

  CProject* project;
  QString preselectitem;
  /** store the long relative filename for every item*/
  QPtrDict <char>* dict;
  
};
#endif








