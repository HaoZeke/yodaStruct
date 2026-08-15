project = "dseams"
release = "2.2.1"
copyright = "2019--present, d-SEAMS core team"
author = "d-SEAMS core team"

extensions = [
    "myst_parser",
    "sphinx.ext.intersphinx",
    "sphinx_sitemap",
    "sphinx_design",
]

templates_path = ["_templates"]
exclude_patterns = []
source_suffix = [".rst", ".md"]
master_doc = "index"

html_theme = "shibuya"
html_static_path = ["_static"]
html_title = "dseams"
html_baseurl = "https://d-seams.github.io/yodaStruct/"

html_context = {
    "source_type": "github",
    "source_user": "d-SEAMS",
    "source_repo": "yodaStruct",
    "source_version": "main",
    "source_docs_path": "/docs/source/",
}

html_sidebars = {
    "**": [
        "sidebars/localtoc.html",
        "sidebars/repo-stats.html",
        "sidebars/edit-this-page.html",
    ],
}

html_theme_options = {
    "github_url": "https://github.com/d-SEAMS/yodaStruct",
    "accent_color": "teal",
    "dark_code": True,
    "nav_links": [
        {
            "title": "Ecosystem",
            "children": [
                {
                    "title": "d-SEAMS engine",
                    "url": "https://docs.dseams.info",
                    "summary": "libyodaLib and the seams CLI",
                },
                {
                    "title": "pydseams",
                    "url": "https://d-seams.github.io/PydSEAMSlib/",
                    "summary": "Python Frame API on yoda",
                },
                {
                    "title": "dseams (Lua)",
                    "url": "https://d-seams.github.io/yodaStruct/",
                    "summary": "require(\"dseams\") and Fennel",
                },
            ],
        },
    ],
}

intersphinx_mapping = {
    "python": ("https://docs.python.org/3", None),
    "dseams": ("https://docs.dseams.info", None),
    "pydseams": ("https://d-seams.github.io/PydSEAMSlib/", None),
}
