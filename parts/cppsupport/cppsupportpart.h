/***************************************************************************
 *   Copyright (C) 1999 by Jonas Nordin                                    *
 *   jonas.nordin@syncom.se                                                *
 *   Copyright (C) 2000-2001 by Bernd Gehrmann                             *
 *   bernd@kdevelop.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _CPPSUPPORTPART_H_
#define _CPPSUPPORTPART_H_

#include "kdevcore.h"
#include "keditor/edit_iface.h"
#include "keditor/cursor_iface.h"

#include "kdevlanguagesupport.h"

#include <kdialogbase.h>
#include <qguardedptr.h>
#include "cppsupportwidget.h"
#include <qstring.h>

class ParsedMethod;
class ParsedAttribute;
class ClassStore;
class Context;
class CClassParser;
class CppCodeCompletion;
class QLabel;
class QProgressBar;

namespace KEditor { class Document; }

class CppSupportPart : public KDevLanguageSupport
{
    Q_OBJECT

public:
    CppSupportPart( bool cpp, KDevApi *api, QObject *parent=0, const char *name=0 );
    ~CppSupportPart();
    
    void setEnableCC( bool b ){ m_bEnableCC = b;    };
    bool getEnableCC( void   ){ return m_bEnableCC; };
    
    /**
     */
    CppSupportWidget* getCHWidget( ){ return m_pCHWidget; };
    
    /**
     * return persistant classstore File Extension
    void slotCompleteText();
    void slotExpandText();
    void slotTypeOfExpression();
     * meant for the project's own classes
     */
    QString pcsFileExt( ){ return ".pcs"; };
    
    /**
     * return pre-parsing File Extension
     * meant for extern includes
     */
    QString ppFileExt ( ){ return "-pp.pcs"; };    
    
protected:
    virtual KDevLanguageSupport::Features features();
    virtual void newClass();
    virtual void addMethod(const QString &className);
    virtual void addAttribute(const QString &className);

private slots:
    void documentActivated(KEditor::Document *doc);
    void projectOpened();
    void projectClosed();
    void savedFile(const QString &fileName);
    void contextMenu(QPopupMenu *popup, const Context *context);
    void addedFileToProject(const QString &fileName);
    void removedFileFromProject(const QString &fileName);

    void slotSwitchHeader();
    void slotGotoIncludeFile();
    void slotCompleteText();
    void slotExpandText();
    void slotTypeOfExpression();

    // Internal
    void initialParse();
    
    // daniel
    void projectConfigWidget( KDialogBase* );
    
private:
    void maybeParse(const QString fileName, ClassStore* pStore, CClassParser* pParser );
    // daniel
    /**
     * parses a directory incl. subdirectories if wanted
     * param directory start directory
     * param withSubDirectoreis true or false
     * param bar progressbar
     * param label for debug output on screen
     */
    void parseDirectory( const QString directory, const bool withSubDirectories, QProgressBar* bar, QLabel* label );
    
    QString asHeaderCode(ParsedMethod *pm);
    QString asCppCode(ParsedMethod *pm);
    QString asHeaderCode(ParsedAttribute *pa);

    CClassParser *m_pParser;
    CppCodeCompletion* m_pCompletion;
    CClassParser* m_pCCParser;
    
    KEditor::CursorDocumentIface* m_pCursorIface;
    KEditor::EditDocumentIface* m_pEditIface;

    bool withcpp;
    QString popupstr;
    
    bool m_bEnableCC;
    KDevCore::Role m_CHSide;
    QGuardedPtr< CppSupportWidget > m_pCHWidget;
    
public slots:
    void slotEnableCodeHinting( bool setEnabled, bool setOutputView );
    void slotEnableCodeCompletion( bool setEnabled );
};

#endif
