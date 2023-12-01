from setuptools import setup

setup(
    name="pyaoch",
    description="Python AoC (Advent of Code) helper script",
    version="1.0.0",
    install_requires=[
        "requests==2.31.0",
        "beautifulsoup4==4.12.2",
        "markdownify==0.11.6"
    ],
    entry_points={
        'console_scripts': [
            'pyaoch = pyaoch.pyaoch:main',
        ]
    }
)
