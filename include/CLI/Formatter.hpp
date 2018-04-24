#pragma once

// Distributed under the 3-Clause BSD License.  See accompanying
// file LICENSE or https://github.com/CLIUtils/CLI11 for details.

#include <string>
#include <map>

#include "CLI/StringTools.hpp"

namespace CLI {

class Option;
class App;

// These items will be exported into classes
namespace detail {

/// This enum signifies what situation the option is beig printed in.
///
/// This is passed in as part of the built in formatter in App; it is
/// possible that a custom App formatter could avoid using it, however.
enum class OptionFormatterMode {
    Usage,      //< In the program usage line
    Positional, //< In the positionals
    Optional    //< In the normal optionals
};

/// This enum signifies the type of help requested
///
/// This is passed in by App; all user classes must accept this as
/// the second argument.

enum class AppFormatterMode {
    Normal, //< The normal, detailed help
    All,    //< A fully expanded help
    Sub,    //<  Used when printed as part of expanded subcommand
};

/// This is an example formatter (and also the default formatter)
/// For option help.
class OptionFormatter {
  public:
    using Mode = OptionFormatterMode;

  protected:
    /// @name Options
    ///@{

    /// @brief The required help printout labels (user changable)
    /// Values are REQUIRED, NEEDS, EXCLUDES
    /// This is a dictionary of strings instead of enums
    /// so a user can extend if needed
    std::map<std::string, std::string> labels_{
        {"REQUIRED", "(REQUIRED)"}, {"NEEDS", "Needs"}, {"EXCLUDES", "Exlucdes"}, {"ENV", "Env"}};

    /// The width of the first column
    size_t column_width_{30};

    ///@}
    /// @name Basic
    ///@{

  public:
    ///@}
    /// @name Setters
    ///@{

    /// Set the "REQUIRED" label
    OptionFormatter *set_label(std::string key, std::string val) {
        labels_[key] = val;
        return this;
    }

    /// Set the column width
    OptionFormatter *set_column_width(size_t val) {
        column_width_ = val;
        return this;
    }

    ///@}
    /// @name Getters
    ///@{

    /// Get the current value of a name (REQUIRED, etc.)
    std::string get_label(std::string key) const { return labels_.at(key); }

    size_t get_column_width() const { return column_width_; }

    ///@}
    /// @name Overridables
    ///@{

    /// @brief This is the name part of an option, Default: left column
    std::string make_name(const Option *, Mode) const;

    /// @brief This is the options part of the name, Default: combined into left column
    std::string make_opts(const Option *, Mode) const;

    /// @brief This is the description. Default: Right column, on new line if left column too large
    std::string make_desc(const Option *, Mode) const;

    /// @brief This is used to print the name on the USAGE line (by App formatter)
    std::string make_usage(const Option *opt) const;

    /// @brief This is the standard help combiner that does the "default" thing.
    std::string operator()(const Option *opt, Mode mode) const {
        std::stringstream out;
        detail::format_help(out, make_name(opt, mode) + make_opts(opt, mode), make_desc(opt, mode), column_width_);
        return out.str();
    }

    ///@}
};

class AppFormatter {
  public:
    using Mode = AppFormatterMode;
    /// @name Options
    ///@{

    /// @brief The required help printout labels (user changable)
    /// Values are REQUIRED, NEEDS, EXCLUDES
    /// This is a dictionary of strings instead of enums
    /// so a user can extend if needed
    std::map<std::string, std::string> labels_{{"USAGE", "Usage"},
                                               {"OPTIONS", "OPTIONS"},
                                               {"SUBCOMMAND", "SUBCOMMAND"},
                                               {"SUBCOMMANDS", "SUBCOMMANDS"},
                                               {"POSITIONALS", "Positionals"}};

    ///@}
    /// @name Basic
    ///@{

  public:
    ///@}
    /// @name Setters
    ///@{

    /// Set the "REQUIRED" label
    AppFormatter *set_label(std::string key, std::string val) {
        labels_[key] = val;
        return this;
    }

    ///@}
    /// @name Getters
    ///@{

    /// Get the current value of a name (REQUIRED, etc.)
    std::string get_label(std::string key) const { return labels_.at(key); }

    /// @name Overridables
    ///@{

    /// This prints out a group of options
    std::string make_group(std::string group, std::vector<Option *> opts, detail::OptionFormatter::Mode mode) const;

    /// This prints out all the groups of options
    std::string make_groups(const App *app) const;

    /// This prints out all the subcommands
    std::string make_subcommands(const App *app, Mode mode) const;

    /// This prints out a subcommand
    std::string make_subcommand(const App *sub, Mode mode) const;

    /// This prints out all the groups of options
    std::string make_footer(const App *app) const;

    /// This displays the description line
    std::string make_description(const App *app) const;

    /// This displays the usage line
    std::string make_usage(const App *app, std::string name) const;

    /// This puts everything together
    std::string operator()(const App *, std::string, Mode) const;
    ///@}
};

} // namespace detail
} // namespace CLI
