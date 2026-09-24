class SceneImage : public SceneObject {
public:
    static std::unique_ptr<SceneImage> load(const std::string & path, float max_dimension = 400.0f);
    
    void draw() const override;

private:
    ofImage image;
};
