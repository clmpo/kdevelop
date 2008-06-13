/* This file is part of KDevelop
    Copyright 2006 Hamish Rodda <rodda@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef TOPDUCONTEXT_H
#define TOPDUCONTEXT_H

#include <QtCore/QMutex>

#include "language/duchain/ducontext.h"

template< class T >
class KSharedPtr;

namespace KDevelop
{
  class QualifiedIdentifier;
  class DUChain;
  class IdentifiedFile; //Defined in parsingenvironment.h
  class ParsingEnvironmentFile;
  class TopDUContextPrivate;
  class Problem;
  class DeclarationChecker;
  class TopDUContext;

  typedef DUChainPointer<TopDUContext> TopDUContextPointer;
  
  typedef KSharedPtr<Problem> ProblemPointer;
/**
 * The top context in a definition-use chain for one source file.
 *
 * Implements SymbolTable lookups and locking for the chain.
 *
 * Contexts and Classes can only be found through TopDUContext if they are in the symbol table.
 * @see DUContext::setInSymbolTable, Declaration::setInSymbolTable 
 *
 * \todo move the registration with DUChain here
 */
class KDEVPLATFORMLANGUAGE_EXPORT TopDUContext : public DUContext
{
public:
  explicit TopDUContext(const HashedString& url, const SimpleRange& range, ParsingEnvironmentFile* file = 0);
  virtual ~TopDUContext();

  TopDUContext* topContext() const;

  /**
   * There may be multiple context's for one file, but each of those should have a different identity().
   *  */
  IdentifiedFile identity() const;

  /**
   * @see ParsingEnvironmentFile
   * May return zero if no file was set.
   * */
  KSharedPtr<ParsingEnvironmentFile> parsingEnvironmentFile() const;
  
  /// Returns true if this object is being deleted, otherwise false.
  bool deleting() const;

  /// Returns true if this object is registered in the du-chain. If it is not, all sub-objects(context, declarations, etc.)
  bool inDuChain() const;
  /// This flag is only used by DUChain, never change it from outside.
  void setInDuChain(bool);

  /**
   * Returns a list of all problems encountered while parsing this top-context.
   * Does not include the problems of imported contexts.
   * */
  QList<ProblemPointer> problems() const;

  /**
   * Add a parsing-problem to this context.
   * */
  void addProblem(const ProblemPointer& problem);
  
  /// Clear the list of problems
  void clearProblems();
  
  /**
   * Determine if this chain imports another chain.
   *
   * \note you must be holding a read but not a write chain lock when you access this function.
   */
  virtual bool imports(const DUContext* origin, const SimpleCursor& position) const;

  /**
   * Returns the trace of imports from this context top the given target.
   * The positions in the returned trace may be invalid.
   * This is more efficient then the version below, because the trace doesn't need to be copied
   * */
  void importTrace(const TopDUContext* target, ImportTrace& store) const;

  ///More convenient version of the above
  ImportTrace importTrace(const TopDUContext* target) const;
  
  enum Flags {
    NoFlags = 0,
    /**
     * A language-specific flag used by C++ to mark one context as a proxy of another.
     * If this flag is set on a context, the first imported context should be used for any computations
     * like searches, listing, etc. instead of using this context.
     *
     * The problems should be stored within the proxy-contexts, and optionally there may be
     * any count of imported proxy-contexts imported behind the content-context(This can be used for tracking problems)
     * 
     * Note: This flag does not directly change the behavior of the language-independent du-chain.
     */
    ProxyContextFlag = 1,
    /**
     * Placeholder used in searching-functions, when the flag should simply be ignored.
     * */
    AnyFlag=2,
    /**
     * Can be used by language parts to mark contexts they currently update(for their internal usage)
     * */
    UpdatingContext = 4,
    LastFlag
  };

  bool hasUses() const;
  void setHasUses(bool hasUses);
  
  /**
   * Retrieves or creates a local index that is to be used for referencing the given @param declaration
   * in local uses. Also registers this context as a user of the declaration.
   * @param create If this is false, only already registered indices will be returned.
   *               If the declaration is not registered, std::limits<int>::max is returned
   *
   * The duchain must be write-locked if create is true, else it must at least be read-locked.
   * */
  int indexForUsedDeclaration(Declaration* declaration, bool create = true);

  /**
   * Tries to retrieve the used declaration @param declarationIndex
   * @param context must be the context where the use happened
   * */
  Declaration* usedDeclarationForIndex(int declarationIndex) const;

  /**
   * You can use this before you rebuild all uses. This does not affect any uses directly,
   * it only invalidates the mapping of declarationIndices to Declarations.
   *
   * usedDeclarationForIndex(..) must not be called until the use has gotten a new index through
   * indexForUsedDeclaration(..).
   * */
  void clearDeclarationIndices();
 
  /**
   * Use flags to mark top-contexts for special behavior. Any flags above LastFlag may be used for language-specific stuff.
   * */
  Flags flags() const;
  void setFlags(Flags f);

  ///@param temporary If this is true, importers of this context will not be notified of the new imports. This greatly increases performance while removing the context,
  ///but creates in inconsistent import-structure. Therefore it is only suitable for temporary imports. These imports will not be visible from contexts that import this one.
  virtual void addImportedParentContext(DUContext* context, const SimpleCursor& position = SimpleCursor(), bool anonymous=false, bool temporary=false);
  ///Use this for mass-adding of imported contexts, it is faster than adding them individually.
  ///@param temporary If this is true, importers of this context will not be notified of the new imports. This greatly increases performance while removing the context,
  ///but creates in inconsistent import-structure. Therefore it is only suitable for temporary imports. These imports will not be visible from contexts that import this one.
  virtual void addImportedParentContexts(const QList<QPair<TopDUContext*, SimpleCursor> >& contexts, bool temporary=false);
  
  virtual void removeImportedParentContext(DUContext* context);
  ///Use this for mass-removing of imported contexts, it is faster than removing them individually.
  virtual void removeImportedParentContexts(const QList<TopDUContext*>& contexts);
  
  class CacheData;
  
  /**The cache allows speeding up repeated searches. When you're planning to do many searches from within the same top-context,
    *this should be done. Just create an item of type Cache, and give it the du-context. The duchain must not be locked when calling this.
    *The cache will automatically be discarded on destruction of the item. It is thread-local, so you don't need to care about multi-threading.
    *@warning The DUChain must not be locked when the constructor is called, and it must not be locked when the destructor is executed.
    *@warning Only use this in phases when you're not changing the Data within this contexts, else the cache will become invalid and search will be fuzzy.
    */
  class Cache {
    public:
    Cache(TopDUContextPointer context);
    ~Cache();
    
    private:
    Q_DISABLE_COPY(Cache);
    CacheData* d;
  };
  
  virtual bool findDeclarationsInternal(const SearchItem::PtrList& identifiers, const SimpleCursor& position, const AbstractType::Ptr& dataType, DeclarationList& ret, const TopDUContext* source, SearchFlags flags) const;
protected:
  void setParsingEnvironmentFile(ParsingEnvironmentFile*);
  
  virtual void findContextsInternal(ContextType contextType, const SearchItem::PtrList& identifiers, const SimpleCursor& position, QList<DUContext*>& ret, SearchFlags flags = NoSearchFlags) const;

  /**
   * Does the same as DUContext::updateAliases, except that it uses the symbol-store, and processes the whole identifier.
   * @param canBeNamespace whether the searched identifier may be a namespace.
   * If this is true, namespace-aliasing is applied to the last elements of the identifiers.
   * */
  template<class Acceptor>
  void applyAliases( const SearchItem::PtrList& identifiers, Acceptor& accept, const SimpleCursor& position, bool canBeNamespace ) const;
  
private:
  
  struct AliasChainElement;
  struct FindDeclarationsAcceptor;
  struct FindContextsAcceptor;
  struct DeclarationChecker;
  struct ContextChecker;

  template<class Acceptor>
  void applyAliases( const AliasChainElement* backPointer, const SearchItem::Ptr& identifier, Acceptor& acceptor, const SimpleCursor& position, bool canBeNamespace ) const;
  //Same as imports, without the slow access-check, for internal usage
  bool importsPrivate(const DUContext * origin, const SimpleCursor& position) const;
  Q_DECLARE_PRIVATE(TopDUContext)
  friend class DUChain; //To allow access to setParsingEnvironmentFile
};

/**
  * Returns all uses of the given declaration within this top-context and all sub-contexts
  * */
QList<SimpleRange> allUses(TopDUContext* context, Declaration* declaration);

/**
  * Returns the smart-ranges of all uses
  * */
QList<KTextEditor::SmartRange*> allSmartUses(TopDUContext* context, Declaration* declaration);

}

#endif // TOPDUCONTEXT_H

// kate: space-indent on; indent-width 2; tab-width 4; replace-tabs on; auto-insert-doxygen on
