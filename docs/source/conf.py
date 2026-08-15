project = "dseams"
release = "2.2.1"
copyright = "2019--present, d-SEAMS core team"
author = "d-SEAMS core team"

extensions = [
    "myst_parser",
    "sphinx.ext.intersphinx",
    "sphinx_sitemap",
]

templates_path = ["_templates"]
exclude_patterns = []
source_suffix = [".rst", ".md"]
master_doc = "index"

html_theme = "shibuya"
html_static_path = ["_static"]
html_title = "dseams"

html_theme_options = {
    "github_url": "https://github.com/d-SEAMS/yodaStruct",
    "accent_color": "teal",
    "dark_code": True,
    "nav_links": [
        {
            "title": "Engine",
            "url": "https://docs.dseams.info",
            "external": True,
        },
        {
            "title": "pydseams",
            "url": "https://d-seams.github.io/PydSEAMSlib/",
            "external": True,
        },
    ],
}

intersphinx_mapping = {
    "python": ("https://docs.python.org/3", None),
}
html_baseurl = "https://d-seams.github.io/yodaStruct/"
