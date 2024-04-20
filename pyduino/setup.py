from setuptools import setup, find_packages

setup(
    name="pyduino_control",
    version="0.1",
    packages=find_packages(),
    install_requires=['pyserial'],
    author="Your Name",
    author_email="your.email@example.com",
    description="A Python module to control one or more Arduino connected to a PC",
    long_description=open("../README.md").read(),
    long_description_content_type="text/markdown",
    url="https://github.com/yourusername/pyduino_control",
    classifiers=[
        "Development Status :: 3 - Alpha",
        "License :: OSI Approved :: MIT License",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.7",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
        "Programming Language :: Python :: 3.11",
        "Programming Language :: Python :: 3.12",

    ],
)
