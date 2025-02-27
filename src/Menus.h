/**********************************************************************

  Audacity: A Digital Audio Editor

  Menus.h

  Dominic Mazzoni

**********************************************************************/
#ifndef __AUDACITY_MENUS__
#define __AUDACITY_MENUS__

#include "Identifier.h"

#include "Prefs.h"
#include "ClientData.h"
#include "commands/CommandFlag.h"
#include "Observer.h"

class wxArrayString;
class wxCommandEvent;
class AudacityProject;
class CommandContext;
class CommandManager;
class Track;
class TrackList;
class ViewInfo;

enum EffectType : int;

typedef wxString PluginID;
typedef wxString MacroID;
typedef wxArrayString PluginIDs;

namespace MenuTable {
   struct Traits;
   template<typename MenuTraits> struct Visitor;
}

class AUDACITY_DLL_API MenuCreator
{
public:
   MenuCreator();
   ~MenuCreator();
   void CreateMenusAndCommands(AudacityProject &project);
   void RebuildMenuBar(AudacityProject &project);

   static void RebuildAllMenuBars();

public:
   CommandFlag mLastFlags;
   
   // Last effect applied to this project
   PluginID mLastGenerator{};
   PluginID mLastEffect{};
   PluginID mLastAnalyzer{};
   int mLastAnalyzerRegistration;
   int mLastAnalyzerRegisteredId;
   PluginID mLastTool{};
   int mLastToolRegistration;
   int mLastToolRegisteredId;
   enum {
      repeattypenone = 0,
      repeattypeplugin = 1,
      repeattypeunique = 2,
      repeattypeapplymacro = 3
   };
   unsigned mRepeatGeneratorFlags;
   unsigned mRepeatEffectFlags;
   unsigned mRepeatAnalyzerFlags;
   unsigned mRepeatToolFlags;
};

//! Sent when menus update (such as for changing enablement of items)
struct MenuUpdateMessage {};

class AUDACITY_DLL_API MenuManager final
   : public MenuCreator
   , public ClientData::Base
   , public Observer::Publisher<MenuUpdateMessage>
   , private PrefsListener
{
public:

   static MenuManager &Get( AudacityProject &project );
   static const MenuManager &Get( const AudacityProject &project );

   explicit
   MenuManager( AudacityProject &project );
   MenuManager( const MenuManager & ) = delete;
   MenuManager &operator=( const MenuManager & ) = delete;
   ~MenuManager();

   static void Visit(
      MenuTable::Visitor<MenuTable::Traits> &visitor, AudacityProject &project);

   static void ModifyUndoMenuItems(AudacityProject &project);

   // checkActive is a temporary hack that should be removed as soon as we
   // get multiple effect preview working
   void UpdateMenus( bool checkActive = true );

   // If checkActive, do not do complete flags testing on an
   // inactive project as it is needlessly expensive.
   CommandFlag GetUpdateFlags( bool checkActive = false ) const;
   void UpdatePrefs() override;

   // Command Handling
   bool ReportIfActionNotAllowed(
      const TranslatableString & Name, CommandFlag & flags, CommandFlag flagsRqd );
   bool TryToMakeActionAllowed(
      CommandFlag & flags, CommandFlag flagsRqd );


private:
   void TellUserWhyDisallowed(const TranslatableString & Name, CommandFlag flagsGot,
      CommandFlag flagsRequired);

   void OnUndoRedo(struct UndoRedoMessage);

   Observer::Subscription mUndoSubscription;
   AudacityProject &mProject;

public:
   // 0 is grey out, 1 is Autoselect, 2 is Give warnings.
   int  mWhatIfNoSelection;
   bool mStopIfWasPaused;
};

#endif
