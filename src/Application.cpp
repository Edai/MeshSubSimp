//
// Created by edai on 19/03/18.
//

#include <fstream>
#include <Mesh.hpp>
#include <Engine.hpp>

Application::Application() = default;

Application::~Application() = default;

bool Application::Parse(Options *options, int ac, char** av)
{
    static struct option long_options[] =
            {
                    {"height", required_argument, nullptr, 'h'},
                    {"width",  required_argument, nullptr, 'w'},
                    {"title",  required_argument, nullptr, 't'},
                    {"object",  required_argument, nullptr, 'o'},
                    {"iteration",  required_argument, nullptr, 'i'},
                    {nullptr, 0,                  nullptr, 0}
            };
    int option_index = 0;
    int c = getopt_long_only(ac, av, "h:w:t:o:i:",
                         long_options, &option_index);
    while (c > 0)
    {
        switch (c)
        {
            case 'h':
                options->height = atoi(optarg);
                break;
            case 'w':
                options->width = atoi(optarg);
                break;
            case 't':
                options->window_name = std::string(optarg);
                break;
            case 'o':
                options->obj_file = std::string(optarg);
                break;
            case 'i':
                options->iterations = atoi(optarg);
                break;
            default:
                return (false);
        }
        c = getopt_long_only(ac, av, "h:w:t:o:i:",
                             long_options, &option_index);
    }
    if (options->window_name.empty())
        options->window_name = std::string(WINDOW_TITLE);
    if (options->width < 1 || options->height < 1)
    {
        options->width = DEFAULT_WIDTH;
        options->height = DEFAULT_HEIGHT;
        std::cout << "Invalid window dimensions... Setting default window dimensions." << std::endl;
    }
    std::ifstream f(options->obj_file.c_str());
    if (!f.good())
    {
        std::cerr << "[" << options->obj_file << "] file does not exist." << std::endl;
        return (false);
    }
    return (true);
}

int Application::MeshProcess(Options *op)
{
    std::cout << "Mesh processing started for the mesh  " << op->obj_file << std::endl;
    Mesh *mesh = new Mesh();
    mesh->Load(op->obj_file.c_str());
    clock_t start = clock();
    for (unsigned i = 0; i < op->iterations; ++i)
    {
        std::cout << "Iteration " << i << std::endl;
        mesh->LoopSubdivisionOneStep();
    }
    clock_t end = clock();
    std::cout << "Time consumed: " << (double)(end - start) / (double)CLOCKS_PER_SEC << std::endl;
    std::string output_file = (op->obj_file.substr(0, op->obj_file.rfind(".")) + ".loop.obj").c_str();
    mesh->Save(output_file.c_str());
    std::cout << "Processing finished, output file : " << output_file << std::endl;
    GraphicalCore::Instance()->meshes.push_back(*mesh);
    return (0);
}

int Application::Start(int ac, char **av)
{
    auto *options = new Options();
    auto *gc = GraphicalCore::Instance();

    if (!Parse(options, ac, av))
        return (-1);
    MeshProcess(options);
    if (!gc->Run(ac, av, options))
        return (-1);
    return (0);
}