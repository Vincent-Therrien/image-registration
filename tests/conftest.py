import pytest

def pytest_addoption(parser):
    parser.addoption("--path", action="store")

@pytest.fixture
def path(request):
    return request.config.getoption("--path")
