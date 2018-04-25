#include <CLI/CLI.hpp>

class MyFormatter : public CLI::Option::Formatter {
  public:
    std::string make_opts(const CLI::Option *) const override { return " OPTION"; }
};

int main(int argc, char **argv) {
    CLI::App app;
    app.set_help_all_flag("--help-all", "Show all help");

    app.option_defaults()->formatter(MyFormatter());

    CLI::App::Formatter fmt;
    fmt.column_width(15);
    app.formatter(fmt);

    app.add_flag("--flag", "This is a flag");

    auto sub1 = app.add_subcommand("one", "Description One");
    sub1->add_flag("--oneflag", "Some flag");
    auto sub2 = app.add_subcommand("two", "Description Two");
    sub2->add_flag("--twoflag", "Some other flag");

    CLI11_PARSE(app, argc, argv);

    std::cout << "This app was meant to show off the formatter, run with -h" << std::endl;

    return 0;
}
