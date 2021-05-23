//
//  FileManager.swift
//  MSLD
//
//  Created by USER on 2021/05/23.
//

import Foundation

enum StorageType {
    case Default
    case fileSystem
}

class PNGFileManager {
    func save(image targetImage: UIImage, forKey key: String) {
        DispatchQueue.global(qos: .background).async {
            self.store(image: targetImage, forKey: key, storageType: .fileSystem)
        }
    }
    
    //MARK: @private
    private func store(image: UIImage, forKey key: String, storageType: StorageType) {
        if let representation = image.pngData() {
            switch storageType {
            case .fileSystem:
                if let filePath = self.getFilePath(forKey: key + ".png") {
                    print(filePath)
                    do {
                        try representation.write(to: filePath, options: .atomic)
                    } catch let error {
                        print("saving results in error %s", error)
                    }
                }
            case .Default:
                // UserDefaults: 데이터 저장소
                // 단일 데이터 값에 적합하며, 대량의 유사한 데이터는 적합하지 않음
                // [데이터, key] 으로 저장
                // https://zeddios.tistory.com/107
                UserDefaults.standard.set(representation, forKey: key)
            }
        }
    }
    
    private func getFilePath(forKey key: String) -> URL? {
        // https://zeddios.tistory.com/440
        let fileManager = FileManager.default
        guard let documentURL = fileManager.urls(for: .documentDirectory, in: .userDomainMask).first else {
            return nil
        }
        return documentURL.appendingPathComponent(key)
    }
}
