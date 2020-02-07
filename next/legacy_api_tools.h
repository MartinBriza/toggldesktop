// Copyright 2014 Toggl Desktop developers.

#ifndef SRC_TOGGL_API_PRIVATE_H_
#define SRC_TOGGL_API_PRIVATE_H_

#include <string>
#include <vector>

#include "model/autotracker.h"
//#include "help_article.h"
//#include "proxy.h"
#include "model/settings.h"
#include "misc/logger.h"
#include "misc/memory.h"

#include "legacy_api.h"

namespace toggl {
class Client;
class Context;
class TimeEntryModel;
class Workspace;
namespace view {
class AutotrackerRule;
class Generic;
class HelpArticle;
class TimeEntry;
}

TOGGL_INTERNAL_EXPORT int compare_string(const char *s1, const char *s2);
TOGGL_INTERNAL_EXPORT const char* to_char(const std::string &s);
TOGGL_INTERNAL_EXPORT char *copy_string(const std::string &s);
TOGGL_INTERNAL_EXPORT std::string to_string(const char *s);

TogglGenericView *generic_to_view_item_list(const std::vector<toggl::view::Generic> &list);
TogglGenericView *generic_to_view_item(const toggl::view::Generic &c);
void view_item_clear(TogglGenericView *item);
void view_list_clear(TogglGenericView *first);

TogglAutotrackerRuleView *autotracker_rule_to_view_item(const toggl::view::AutotrackerRule &model);
void autotracker_view_item_clear(TogglAutotrackerRuleView *view);
void autotracker_view_list_clear(TogglAutotrackerRuleView *first);

/*
TogglAutocompleteView *autocomplete_list_init(std::vector<toggl::view::Autocomplete> *items);
TogglAutocompleteView *autocomplete_item_init(const toggl::view::Autocomplete &item);
void autocomplete_item_clear(TogglAutocompleteView *item);
void autocomplete_list_clear(TogglAutocompleteView *first);
*/

TogglCountryView *country_list_init(std::vector<TogglCountryView> *items);
void country_item_clear(TogglCountryView *item);
void country_list_clear(TogglCountryView *first);
TogglCountryView *country_view_item_init(const Json::Value v);

TogglTimeEntryView *time_entry_view_item_init(locked<const toggl::TimeEntryModel> &te);
void time_entry_view_item_clear(TogglTimeEntryView *item);
void time_entry_view_list_clear(TogglTimeEntryView *first);

TogglSettingsView *settings_view_item_init(const bool_t record_timeline, const toggl::SettingsModel &settings, const bool_t use_proxy);
void settings_view_item_clear(TogglSettingsView *view);

/*
TogglHelpArticleView *help_article_list_init(const std::vector<toggl::HelpArticleModel> &items);
void help_article_item_clear(TogglHelpArticleView *view);
void help_article_list_clear(TogglHelpArticleView *first);
*/

TogglTimelineChunkView *timeline_chunk_view_init(const time_t &start);
void timeline_chunk_view_clear(TogglTimelineChunkView *first);

TogglTimelineEventView *timeline_event_view_init(const toggl::TimelineEventModel &event);
void timeline_event_view_update_duration(TogglTimelineEventView *event_view, const int64_t duration);
void timeline_event_view_clear(TogglTimelineEventView *event_view);

toggl::Logger logger();

toggl::Context *app(void *context);

}  // namespace toggl

#endif  // SRC_TOGGL_API_PRIVATE_H_
