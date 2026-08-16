import os
import subprocess

_conf_dir = os.path.dirname(os.path.abspath(__file__))
subprocess.check_call(["doxygen", "Doxyfile"], cwd=_conf_dir)

project = "dseams"
release = "2.2.5"
copyright = "2019--present, d-SEAMS core team"
author = "d-SEAMS core team"

extensions = [
    "breathe",
    "sphinx.ext.intersphinx",
    "sphinx_sitemap",
    "sphinx_design",
    "sphinxcontrib.mermaid",
]

breathe_projects = {"dseams": "xml"}
breathe_default_project = "dseams"

templates_path = ["_templates"]
exclude_patterns = []
source_suffix = [".rst"]
master_doc = "index"

html_theme = "shibuya"
html_static_path = ["_static"]
html_css_files = []  # sphinx-design ships its own CSS
html_js_files = []
html_title = "dseams"
html_baseurl = "https://d-seams.github.io/yodaStruct/"

html_context = {
    "source_type": "github",
    "source_user": "d-SEAMS",
    "source_repo": "yodaStruct",
    "source_version": "main",
    "source_docs_path": "/docs/source/",
}

# Mermaid: use default CDN; diagrams authorable via ``.. mermaid::`` (from Org RST export).
mermaid_version = "11.4.0"
mermaid_init_js = "mermaid.initialize({startOnLoad:true, theme:'neutral'});"

html_theme_options = {
    "github_url": "https://github.com/d-SEAMS/yodaStruct",
    "accent_color": "teal",
    "dark_code": True,
    "globaltoc_expand_depth": 1,
    "toctree_collapse": True,
    "toctree_maxdepth": 3,
    "toctree_titles_only": True,
    "nav_links": [
        {
            "title": "Ecosystem",
            "children": [
                {
                    "title": "d-SEAMS engine",
                    "url": "https://docs.dseams.info",
                    "summary": "libyodaLib and the seams CLI",
                    "external": True,
                },
                {
                    "title": "pydseams",
                    "url": "https://d-seams.github.io/PydSEAMSlib/",
                    "summary": "Python Frame API on yoda",
                    "external": True,
                },
                {
                    "title": "dseams (Lua)",
                    "url": "https://d-seams.github.io/yodaStruct/",
                    "summary": "require(\"dseams\") and Fennel",
                    "external": True,
                },
            ],
        },
    ],
}

html_sidebars = {
    "**": [
        "sidebars/localtoc.html",
        "sidebars/repo-stats.html",
        "sidebars/edit-this-page.html",
    ],
}

intersphinx_mapping = {
    "python": ("https://docs.python.org/3", None),
    "dseams": ("https://docs.dseams.info", None),
    "pydseams": ("https://d-seams.github.io/PydSEAMSlib/", None),
}
